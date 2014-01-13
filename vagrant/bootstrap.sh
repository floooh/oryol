#
#   Get some bare-bones essentials necessary for continuing with puppet.
#

if [ ! -f pkg/puppetlabs-release-precise.deb ]; then
    mkdir -p pkg
    cd pkg
    wget https://apt.puppetlabs.com/puppetlabs-release-precise.deb
    dpkg -i puppetlabs-release-precise.deb
    apt-get autoremove
    apt-get clean
    apt-get update
    apt-get install -y python-software-properties
    add-apt-repository -y ppa:chris-lea/node.js
    apt-get autoremove
    apt-get clean
    apt-get update
    apt-get install -y puppet-common
    cd ..
fi

