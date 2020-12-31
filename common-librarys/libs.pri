linux {
    linux-g++ | linux-g++-64 | linux-g++-32 {
        CONFIG += LinuxBuild
        DEFINES += __STDC_LIMIT_MACROS
    } else : linux-rasp-pi2-g++ {
        CONFIG += LinuxBuild
        DEFINES += __STDC_LIMIT_MACROS __rasp_pi2__
    } else : android-g++ {
        CONFIG += AndroidBuild MobileBuild
        DEFINES += __android__
        DEFINES += __STDC_LIMIT_MACROS
        DEFINES += QGC_ENABLE_BLUETOOTH
        target.path = $$DESTDIR
    } else {
        error("Unsuported Linux toolchain, only GCC 32- or 64-bit is supported")
    }
} else : win32 {
    win32-msvc2010 | win32-msvc2012 | win32-msvc2013 | win32-msvc2015 {
        CONFIG += WindowsBuild
        DEFINES += __STDC_LIMIT_MACROS
    } else {
        error("Unsupported Windows toolchain, only Visual Studio 2010, 2012, 2013 and 2015 are supported")
    }
} else : macx {
    macx-clang | macx-llvm {
        CONFIG += MacBuild
        DEFINES += __macos__
        CONFIG += x86_64
        CONFIG -= x86
equals(QT_MAJOR_VERSION, 5) | greaterThan(QT_MINOR_VERSION, 5) {
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
} else {
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6
}
        QMAKE_MAC_SDK = macosx10.11
        QMAKE_CXXFLAGS += -fvisibility=hidden
    } else {
        error("Unsupported Mac toolchain, only 64-bit LLVM+clang is supported")
    }
} else : ios {
    !equals(QT_MAJOR_VERSION, 5) | !greaterThan(QT_MINOR_VERSION, 4) {
        error("Unsupported Qt version, 5.5.x or greater is required for iOS")
    }
    message("iOS build")
    CONFIG += iOSBuild MobileBuild app_bundle
    DEFINES += __ios__
    QMAKE_IOS_DEPLOYMENT_TARGET = 8.0
    QMAKE_IOS_TARGETED_DEVICE_FAMILY = 1,2 # Universal
    QMAKE_LFLAGS += -Wl,-no_pie
} else {
    error("Unsupported build platform, only Linux, Windows, Android and Mac (Mac OS and iOS) are supported")
}

MobileBuild {
    DEFINES += __mobile__
}

# Installer configuration

installer {
    CONFIG -= debug
    CONFIG -= debug_and_release
    CONFIG += release
    message(Build Installer)
}

# Setup our supported build flavors

CONFIG(debug, debug|release) {
    message(Debug flavor)
    CONFIG += DebugBuild
} else:CONFIG(release, debug|release) {
    message(Release flavor)
    CONFIG += ReleaseBuild
} else {
    error(Unsupported build flavor)
}

# Setup our build directories

SOURCEDIR      = $$IN_PWD

!iOSBuild {
    OBJECTS_DIR  = $${OUT_PWD}/obj
    MOC_DIR      = $${OUT_PWD}/moc
    UI_DIR       = $${OUT_PWD}/ui
    RCC_DIR      = $${OUT_PWD}/rcc
}

LANGUAGE = C++

# Turn off serial port warnings
DEFINES += _TTY_NOWARN_

#
# By default warnings as errors are turned off. Even so, in order for a pull request
# to be accepted you must compile cleanly with warnings as errors turned on the default
# set of OS builds. See http://www.qgroundcontrol.org/dev/contribute for more details.
# You can use the WarningsAsErrorsOn CONFIG switch to turn warnings as errors on for your
# own builds.
#

MacBuild | LinuxBuild {
    QMAKE_CXXFLAGS_WARN_ON += -Wall
    WarningsAsErrorsOn {
        QMAKE_CXXFLAGS_WARN_ON += -Werror
    }
}

WindowsBuild {
    QMAKE_CXXFLAGS_WARN_ON += /W3 \
        /wd4996 \   # silence warnings about deprecated strcpy and whatnot
        /wd4005 \   # silence warnings about macro redefinition
        /wd4290 \   # ignore exception specifications
        /Zc:strictStrings-  # work around win 8.1 sdk sapi.h problem
    WarningsAsErrorsOn {
        QMAKE_CXXFLAGS_WARN_ON += /WX
    }
}

#
# Build-specific settings
#

ReleaseBuild {
    DEFINES += QT_NO_DEBUG
    WindowsBuild {
        # Use link time code generation for better optimization (I believe this is supported in MSVC Express, but not 100% sure)
        QMAKE_LFLAGS_LTCG = /LTCG
        QMAKE_CFLAGS_LTCG = -GL

        # Turn on debugging information so we can collect good crash dumps from release builds
        QMAKE_CXXFLAGS_RELEASE += /Zi
        QMAKE_LFLAGS_RELEASE += /DEBUG
    }
}

#
# Unit Test specific configuration goes here
#

DebugBuild {
    DEFINES += UNITTEST_BUILD
}

DebugBuild: {
    OUT_ROOT = $${OUT_PWD}/../../output/debug/
}
else: {
    OUT_ROOT = $${OUT_PWD}/../../output/release/
}
