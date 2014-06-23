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

update_todo()
{
    find . \( -name \*.h -o -name \*.cpp \) -exec grep TODO {} + \
        | sed "s/^\(.\+.[cpp|h]:\).*\(TODO.*\)$$/\1 \2/" > TODO.list
}

run_valgrind()
{
    DISPLAY=:1 valgrind --tool=memcheck --leak-check=full  \
               --track-origins=yes --suppressions=supp/xlib.supp \
               --suppressions=supp/libxft.supp \
               --show-reachable=yes ./main
              #--gen-suppressions=all --log-file=libxft.supp \
              #--show-reachable=yes xinit ./main -- :1
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

        --valgrind)
            echo "Running valgrind"
            run_valgrind
            ;;

        --todo)
            echo "Updating TODO file"
            update_todo
            ;;

        *)
            echo "Invalid option $option"
            exit
            ;;

    esac
}

main "$1" "$2"
