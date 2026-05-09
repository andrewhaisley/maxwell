# maxwell

## easy install
To use, install the .deb package from this repo:

sudo dpkg -i maxwell-0.1-Linux.deb

And then run the exe that should be in /usr/local/bin

maxwell

Note that the first time you run it, startup will be slow due to font metric caching.

## build from source

Clone the repo, then:

cmake .
make

./maxwell

## config files

On first run, Maxwell creates a collection of files in ~/.maxwellrc
