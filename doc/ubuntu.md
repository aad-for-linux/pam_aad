# Ubuntu

## Dependencies

To install on Ubuntu, the following personal package archives (PPAs),
must be installed:

- [Simple Dynamic Strings][sds-ppa]
- [Azure Active Directory for Linux][aad-for-linux-ppa]

These can be installed via the following commands:

```terminal
# Simple Dynamic Strings
add-apt-repository ppa:lramage/sds

# Azure Active Directory for Linux
add-apt-repository ppa:aad-for-linux/ppa
```

Note: __Both__ PPA's must be installed in order for all of the dependencies to be met.

## Installation

```terminal
apt update && apt install -y libpam-aad
```

## Configuration

The package provided should automatically install the following configuration files:

- [`/etc/pam_aad.conf`](../debian/pam_aad.conf)
- [`/usr/share/pam-configs/aad`](../debian/pam-configs/aad)

## Troubleshooting

If `add-apt-repository` is missing from your system, i.e., a docker image,
it can be installed via the following command:

```terminal
apt install -y software-properties-common
```

If the key for the PPA fails to import properly, 
it can be manually added via the following command:

```terminal
# Simple Dynamic Strings
apt-key adv --keyserver keyserver.ubuntu.com --recv A2C0BCEBD63BA8C1
```

[aad-for-linux-ppa]: https://launchpad.net/~aad-for-linux/+archive/ubuntu/ppa
[sds-ppa]: https://launchpad.net/~lramage/+archive/ubuntu/sds
