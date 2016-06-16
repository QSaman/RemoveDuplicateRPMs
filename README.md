# RemoveDuplicateRPMs
Suppose you want to keep RPMs, in Fedora when you update a piece of software, the package manager doesn't remove the old ones. So after some time your RPMs take a lot of space. This program removes old RPMs by querying RPM databas.
## Usage
You can use below command. "dir-path" is the directory which contains all downloaded RPMs. If after that you use "remove", then all old RPMs will be deleted.
```
RemoveDuplicateRPMs dir-path [remove]
```
For example if you only want to list the old RPMs, use below command:

```
RemoveDuplicateRPMs dir-path /path/to/rpm/directory
```
and when everything is OK and you want to delete them:

```
RemoveDuplicateRPMs dir-path /path/to/rpm/directory remove
```
