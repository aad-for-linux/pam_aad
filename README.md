# pam_aad

[![GPL-3.0-or-later][gpl-badge]][gpl-license]

Azure Active Directory PAM Module

_This PAM module aims to provide Azure Active Directory authentication for Linux._

##  Installation

```
./bootstrap.sh
./configure --with-pam-dir=/lib/x86_64-linux-gnu/security/
make
sudo make install
```

## Configuration

Edit `/etc/pam.d/{{service}}` and add the following line:

```
auth required pam_aad.so
``` 

### Configuration File

Create the file ```/etc/pam_aad.conf``` and fill it with:

```mustache
{ 
  "client": {
    "id": "{{client_id}}"
  },
  "domain": "{{domain}}",
  "group": {
    "id": "{{group_id}}"
  },
  "smtp_server": "{{smtp_server}}",
  "tenant": {
    "name": "{{organization}}.onmicrosoft.com",
    "address": "{{organization_email_address}}"
  }
}
```

## Current Behavior

[![asciicast](https://asciinema.org/a/250072.svg)](https://asciinema.org/a/250072)

## See also

- https://github.com/google/google-authenticator-libpam
- https://github.com/quarxConnect/pam_oauth2

[gpl-badge]: https://img.shields.io/badge/license-GPL-green.svg
[gpl-license]: COPYING
