# Mahimahi

A web performance measurement toolkit with addtional GE loss model implemented.

## GE Loss Model (Gilbert–Elliott Loss)

Mahimahi traditionally includes IID loss model.
We extend Mahimahi with a **Gilbert–Elliott (GE) bursty loss model**, which is
commonly used to simulate real network loss patterns that exhibit temporal
correlation (bursty loss).

The GE model is a two-state Markov chain:

- **Good state (G)** – packets are dropped with probability `p_good`
- **Bad state (B)** – packets are dropped with probability `p_bad`
- State transitions:
  - `G → B` with probability `p_gb`
  - `B → G` with probability `p_bg`

This allows Mahimahi to simulate correlated loss bursts and recoveries, unlike
the basic IID loss model.

The GE loss shell is provided by the binary `mm-ge-loss`. It works similarly to the default `mm-loss` tool

```bash
mm-ge-loss uplink p_gb p_bg p_good p_bad
```

## Install

### Requirements

On Ubuntu (at least), you need the following packages to install mahimahi:
 * make
 * autoconf
 * libtool
 * iproute2
 * iptables
 * dnsmasq
 * apache2
 * apache2-dev
 * protobuf-compiler
 * pkg-config
 * libssl-dev
 * libxcb-present-dev
 * libpangomm-2.48-dev

### Install

Once all dependencies are met, you can install mahimahi by running:
 * `./autogen.sh`
 * `./configure`
 * `make`
 * `sudo make install`

### Usage

see [mahimahi usage](http://mahimahi.mit.edu/#usage)

Example:
```bash
mm-delay 50 mm-link trace_up.pps trace_down.pps mm-ge-loss uplink 0.05 0.8 0.02 0.5
```

### Notes

**Mahimahi shell prefix**:

Mahimahi will modify `$MAHIMAHI_SHELL_PREFIX` and `$PS1` to display the current status. If you use `zsh`, it may not display correctly because zsh does not use `$PS1` in the same way. You can manually run `echo $MAHIMAHI_SHELL_PREFIX` to see the status.

**How to access internet from mahimahi box**:

1. Setup NAT

```bash
sudo iptables -t nat -I POSTROUTING 1 -s 10.0.0.0/24 -j MASQUERADE
```

2.  Setup FORWARD

```bash
sudo iptables -I FORWARD 1 -i delay+ -j ACCEPT
sudo iptables -I FORWARD 1 -o delay+ -j ACCEPT
sudo iptables -I FORWARD 1 -i loss+ -j ACCEPT
sudo iptables -I FORWARD 1 -o loss+ -j ACCEPT
sudo iptables -I FORWARD 1 -i link+ -j ACCEPT
sudo iptables -I FORWARD 1 -o link+ -j ACCEPT
```
