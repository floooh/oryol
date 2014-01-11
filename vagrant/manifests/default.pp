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

$clang_name = "clang+llvm-3.2-x86-linux-ubuntu-12.04"
$clang_tar  = "${clang_name}.tar.gz"
$clang_url = "http://llvm.org/releases/3.2/${clang_tar}"

$nacl_name = "nacl_sdk"
$nacl_zip  = "${nacl_name}.zip"
$nacl_url = "http://storage.googleapis.com/nativeclient-mirror/nacl/nacl_sdk/${nacl_zip}"

# run all commands as vagrant user
Exec {
    user => "${user}",
    logoutput => on_failure,
    timeout => 0
}

# essential packages
class essentials {

    file { "${pkg}":
        mode => 775,
        owner => "${user}",
        ensure => directory
    }
    
    package { "libc6:i386": }
    package { "libstdc++6:i386": }
    package { "build-essential": }
    package { "vim": }
    package { "git": }
    package { "unzip": }
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
        creates => "/usr/local/bin/cmake"
    }
}

# native Linux build requirements
class oryol-build {
    package { "libcurl4-gnutls-dev": }
    package { "uuid-dev": }
}

# get clang-llvm 3.2 for emscripten
class clang {

    exec { "/usr/bin/wget ${clang_url}":
        alias => "get-clang",
        cwd => "${pkg}",
        creates => "${pkg}/${clang_tar}"        
    }

    exec { "/bin/tar -xvf ${clang_tar}":
        alias => "untar-clang",
        cwd => "${pkg}",
        require => Exec["get-clang"],
        creates => "${pkg}/${clang_name}/bin/clang"
    }
}

# emscripten SDK
class emscripten {

    require essentials
    
    $deps = [ "default-jre", "nodejs" ]

    package { $deps: }

    exec { "/usr/bin/git clone https://github.com/floooh/emscripten.git":
        alias => "git-clone-emscripten",
        cwd => "${home}",
        creates => "${home}/emscripten/emcc",
    }

    exec { "/usr/bin/git checkout incoming":
        alias => "git-checkout-emscripten",
        cwd => "${home}/emscripten",
        require => Exec["git-clone-emscripten"],
    }

    file { "${home}/.emscripten":
        mode => 664,
        source => "${files}/dot.emscripten"
    }
}

# PNaCl SDK
class nacl-sdk {

    require essentials
    
    exec { "/usr/bin/wget ${nacl_url}":
        alias => "get-nacl",
        cwd => "${pkg}",
        creates => "${pkg}/${nacl_zip}"        
    }

    exec { "/usr/bin/unzip ${nacl_zip} -d ..":
        alias => "unzip-nacl",
        cwd => "${pkg}",
        require => Exec["get-nacl"],
        creates => "${home}/${nacl_name}/naclsdk"
    }

    exec { "${home}/${nacl_name}/naclsdk update pepper_canary":
        alias => "update-nacl",
        cwd => "${home}",
        require => Exec["unzip-nacl"],
        creates => "${home}/${nacl_name}/pepper_canary/README",
    }
}

include essentials
include ninja
include cmake
include oryol-build
include clang
include emscripten
include nacl-sdk

