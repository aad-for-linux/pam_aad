# pam_aad

[![GPL-3.0-or-later](https://img.shields.io/badge/license-GPL--3.0--or--later-blue?style=flat-square)](https://spdx.org/licenses/GPL-3.0-or-later.html)
[![GitHub Actions](https://img.shields.io/github/workflow/status/aad-for-linux/pam_aad/build?style=flat-square)](https://github.com/aad-for-linux/pam_aad/actions)

_Azure Active Directory PAM Module._

##  Installation

```
./bootstrap.sh
./configure
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

