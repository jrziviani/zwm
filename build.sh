#!/bin/sh

generate_configure()
{
    aclocal
    autoconf
    automake -a -c
}

configure_make()
{
    local folder="$1"
    local prefix=""

    [[ "$profile" != "none" ]] && prefix="--prefix=$folder"

    ./configure $prefix
    make
}

force_build()
{
    make mostlyclean
}

clean_configure()
{
    make distclean
    make maintainer-clean
    rm -fr aclocal.m4 \
        configure \
        depcomp \
        install-sh \
        Makefile.in \
        missing
}

main()
{
    local option="$1"

    case "$option" in

        --make)
            echo "Creating ZWM binaries"

            folder=${2-none}

            generate_configure
            configure_make "$folder"
            ;;

        --force-build)
            echo "Rebuilding"
            force_build
            make
            ;;

        --clean-all)
            echo "Clean project"
            clean_configure
            ;;

        *)
            echo "Invalid option $option"
            exit
            ;;

    esac
}

main "$1" "$2"
