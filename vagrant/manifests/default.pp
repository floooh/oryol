#
#   Puppet manifest to setup Oryol dev environment
#   for compiling to native Linux, emscripten and PNaCl
#

$user = "vagrant"
$home = "/home/vagrant"
$pkg = "${home}/pkg"
$files = "/vagrant/files"

$cmake_name = "cmake-2.8.12.1"
$cmake_tar  = "${cmake_name}.tar.gz"
$cmake_url  = "http://www.cmake.org/files/v2.8/${cmake_tar}"

# run all commands as vagrant user
Exec {
    user => "${user}",
    logoutput => true,
    timeout => 0
}

# essential packages
class essentials {

    file { "${pkg}":
        mode => 775,
        owner => "${user}",
        ensure => directory
    }
    
    package { "lib32z1": }
    package { "zlib1g": }
    package { "libc6:i386": }
    package { "libstdc++6:i386": }
    package { "build-essential": }
    package { "vim": }
    package { "git": }
    package { "unzip": }
    package { "cmake-curses-gui": }
    package { "default-jre": }
    package { "ant": }
    package { "nodejs": }
}

# ninja build-system
class ninja {

    require essentials
    
    exec { "/usr/bin/git clone git://github.com/martine/ninja.git":
        alias => "git-clone-ninja",
        cwd => "${pkg}",
        creates => "${pkg}/ninja/bootstrap.py",
    }

    exec { "/usr/bin/git checkout release":
        alias => "git-checkout-ninja",
        cwd => "${pkg}/ninja",
        require => Exec["git-clone-ninja"],
    }

    exec { "${pkg}/ninja/bootstrap.py":
        alias => "ninja-bootstrap",
        cwd => "${pkg}/ninja",
        require => Exec["git-checkout-ninja"],
        creates => "${pkg}/ninja/ninja",
    }

    file { "/usr/local/bin/ninja":
        ensure => "link",
        target => "${pkg}/ninja/ninja",
        require => Exec["ninja-bootstrap"],
    }
}

# recent CMake version (std package is too old)
class cmake {

    require essentials
    
    exec { "/usr/bin/wget ${cmake_url}":
        alias => "wget-cmake",
        cwd => "${pkg}",
        creates => "${pkg}/${cmake_tar}",
    }

    exec { "/bin/tar -xvf ${cmake_tar}":
        alias => "untar-cmake",
        require => Exec["wget-cmake"],
        cwd => "${pkg}",
        creates => "${pkg}/${cmake_name}/Readme.txt"
    }

    exec { "${pkg}/${cmake_name}/configure":
        alias => "config-cmake",
        require => Exec["untar-cmake"],
        cwd => "${pkg}/${cmake_name}",
        creates => "${pkg}/${cmake_name}/Makefile",
    }

    exec { "/usr/bin/make -j2":
        alias => "build-cmake",
        require => Exec["config-cmake"],
        cwd => "${pkg}/${cmake_name}",
        creates => "${pkg}/${cmake_name}/bin/cmake"
    }

    exec { "/usr/bin/make install":
        alias => "install-cmake",
        require => Exec["build-cmake"],
        cwd => "${pkg}/${cmake_name}",
        creates => "/usr/local/bin/cmake",
        user => "root"
    }
}

# native Linux build requirements
class oryol-build {
    package { "libcurl4-gnutls-dev": }
    package { "uuid-dev": }
    package { "libglu1-mesa-dev": }
    package { "xorg-dev": }
}

# need gcc4.8 for C++11
class gcc {
    package { "gcc-4.8": }
    package { "g++-4.8": }

    exec { "/usr/sbin/update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 50":
        require => Package["gcc-4.8"],
        user => "root"
    }
    exec { "/usr/sbin/update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 50": 
        require => Package["g++-4.8"],
        user => "root"
    }
    exec { "/usr/sbin/update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-4.8 50":
        require => Package["g++-4.8"],
        user => "root"
    }
}

include essentials
include ninja
include cmake
include oryol-build
include gcc

