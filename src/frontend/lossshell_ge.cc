/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>

#include <getopt.h>

#include "loss_queue.hh"
#include "util.hh"
#include "ezio.hh"
#include "packetshell.cc"

using namespace std;

void usage( const string & program_name )
{
    throw runtime_error(
        "Usage: " + program_name +
        " uplink|downlink p_gb p_bg p_good p_bad [COMMAND...]" );
}

int main( int argc, char *argv[] )
{
    try {
        const bool passthrough_until_signal = getenv( "MAHIMAHI_PASSTHROUGH_UNTIL_SIGNAL" );

        /* clear environment while running as root */
        char ** const user_environment = environ;
        environ = nullptr;

        check_requirements( argc, argv );

        if ( argc < 6 ) {
            usage( argv[ 0 ] );
        }

        const string link = argv[ 1 ];

        double p_gb   = myatof( argv[ 2 ] );   // Good -> Bad transition
        double p_bg   = myatof( argv[ 3 ] );   // Bad -> Good transition
        double p_good = myatof( argv[ 4 ] );   // loss prob in Good state
        double p_bad  = myatof( argv[ 5 ] );   // loss prob in Bad state

        auto valid_prob = [](double x){
            return (0.0 <= x && x <= 1.0);
        };

        if ( !valid_prob(p_gb) || !valid_prob(p_bg) ||
             !valid_prob(p_good) || !valid_prob(p_bad) ) {
            throw runtime_error(
                "Error: all probability parameters must be between 0 and 1."
            );
        }

        double uplink_gb = 0, uplink_bg = 0, uplink_good = 0, uplink_bad = 0;
        double downlink_gb = 0, downlink_bg = 0, downlink_good = 0, downlink_bad = 0;

        if ( link == "uplink" ) {
            uplink_gb   = p_gb;
            uplink_bg   = p_bg;
            uplink_good = p_good;
            uplink_bad  = p_bad;
        }
        else if ( link == "downlink" ) {
            downlink_gb   = p_gb;
            downlink_bg   = p_bg;
            downlink_good = p_good;
            downlink_bad  = p_bad;
        }
        else {
            usage( argv[ 0 ] );
        }

        vector<string> command;

        if ( argc == 6 ) {
            command.push_back( shell_path() );
        } else {
            for ( int i = 6; i < argc; i++ ) {
                command.push_back( argv[i] );
            }
        }

        PacketShell<GELoss> loss_app( "ge-loss",
                                      user_environment,
                                      passthrough_until_signal );

        /* build shell prefix */
        string shell_prefix = "[ge-loss ";
        shell_prefix += link + " ";
        shell_prefix += "p_gb=" + string(argv[2]) + " ";
        shell_prefix += "p_bg=" + string(argv[3]) + " ";
        shell_prefix += "p_good=" + string(argv[4]) + " ";
        shell_prefix += "p_bad=" + string(argv[5]) + "] ";

        loss_app.start_uplink(
            shell_prefix,
            command,
            uplink_gb, uplink_bg, uplink_good, uplink_bad);
            
        loss_app.start_downlink(
            downlink_gb, downlink_bg,
            downlink_good, downlink_bad);

        return loss_app.wait_for_exit();
    }
    catch ( const exception & e ) {
        print_exception( e );
        return EXIT_FAILURE;
    }
}