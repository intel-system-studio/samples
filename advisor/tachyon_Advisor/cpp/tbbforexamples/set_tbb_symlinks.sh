#!/bin/bash
 
# Copyright 2005-2010 Intel Corporation.  All Rights Reserved.
#
# The source code contained or described herein and all documents related
# to the source code ("Material") are owned by Intel Corporation or its
# suppliers or licensors.  Title to the Material remains with Intel
# Corporation or its suppliers and licensors.  The Material is protected
# by worldwide copyright laws and treaty provisions.  No part of the
# Material may be used, copied, reproduced, modified, published, uploaded,
# posted, transmitted, distributed, or disclosed in any way without
# Intel's prior express written permission.
#
# No license under any patent, copyright, trade secret or other
# intellectual property right is granted to or conferred upon you by
# disclosure or delivery of the Materials, either expressly, by
# implication, inducement, estoppel or otherwise.  Any license under such
# intellectual property rights must be express and approved by Intel in
# writing.

ERR_OK=0
ERR_UNKNOWN_ARC=-1

ARC_GET(){
	[ "${PLATFORM}x" = "x" ] || return ${ERR_OK}
	arch_tool="uname -m"
	if $($arch_tool | egrep 'i.86' > /dev/null) ; then
		IA='IA32';
		PLATFORM=ia32;
	elif $($arch_tool | grep ia64 > /dev/null) ; then 
		IA='IA64'
		PLATFORM=ia64
	elif $($arch_tool | grep x86_64 > /dev/null) ; then
		IA='INTEL64'
		PLATFORM=intel64
	else
		# don't know anything about your arc
		return ${ERR_UNKNOWN_ARC}
	fi # if [ arch | egrep 'i?86' > /dev/null ] ; then
	return ${ERR_OK}
} # ARC_GET(){

#############################################################
report_error()
{
    echo "ERROR: $1"
    echo "Build will be canceled. Press ENTER key..."
    echo "Press Enter to continue."
    read
    exit 1
}

check_for_valid_platform_error()
{
    report_error $1
    exit 1
}

check_for_valid_platform_warning()
{
    platform_spec=
    had_check_platform_warnings=true
    echo "WARNING: $1"  
    echo "    Since the platform could not be determined, bin/ and lib/ links will" 
    echo "    not be created." 
    echo 
    echo "    Libraries for a number of supported platforms are available in the"    
    echo "        tbbforexamples/{ia32,intel64}/<platform>/{bin,lib}/"           
    echo "    product directories, where <platform> describes"    
    echo "    a particular supported compiler and OS version.  To use one of these"     
    echo "    platforms, create a symlink for tbbforexample/lib/{ia32,intel64} and" 
    echo "    tbbforexample/lib/{ia32,intel64} to one of these directories"

}
				    
check_for_valid_platform_linux_mac_gcc()
{
    gcc_version_full=`gcc --version | grep "gcc"| egrep -o " [0-9]+\.[0-9]+\.[0-9]+.*" | sed -e s/^\ //`
    if [ $? -eq 0 ]; then
      gcc_version=`echo "$gcc_version_full" | egrep -o "^[0-9]+\.[0-9]+\.[0-9]+"`
      if [ -n "$gcc_version" ]; then return; fi  # got it
    fi
    if [ -n "$get_platform_actual_switch" ]; then
        check_for_valid_platform_error "could not determine platform (could not determine gcc version)$1"
    else
        check_for_valid_platform_warning "could not determine platform (could not determine gcc version)$1"
    fi
				
}

check_for_valid_platform_linux()
{
    if [ "$platform" != "linux" ]; then check_for_valid_platform_error "unsupported platform: platform=$platform"; fi
    
    if [ -n "$get_platform_actual_switch" -a -n "$TBB_ARCH" ]; then
        case "$TBB_ARCH" in
            ia32 | intel64 | ia64 ) processor="$TBB_ARCH" ;;
            * ) check_for_valid_platform_error "unsupported architecture: TBB_ARCH=$TBB_ARCH" ;;
        esac
    else
        my_arch=`uname -m`
        case "$my_arch" in
	    i386 | i686 )  processor="ia32" ;;
	    x86_64 )       processor="intel64" ;;
	    ia64 )         processor="ia64" ;;
	    * ) check_for_valid_platform_error "unsupported architecture: arch=$my_arch" ;;
	esac
    fi
																										
    check_for_valid_platform_linux_mac_gcc

    os_kernel_version_full=`uname -r`
    os_kernel_version=`echo "$os_kernel_version_full" | sed -e 's/-.*$//'`
    os_glibc_version_full=`getconf GNU_LIBC_VERSION | grep glibc | sed -e 's/^glibc //'`
    os_glibc_version=`echo "$os_glibc_version_full" | sed -e '2,$d' -e 's/-.*$//'`
		
    platform_spec="cc${gcc_version}_libc${os_glibc_version}_kernel${os_kernel_version}"

    orig_platform_spec="$platform_spec"
    
    # Linux supported-platform (and remap-platform) table.

    case "${processor}/${platform_spec}" in
    	ia32/cc3.[01][._]* | ia32/cc[012]* )
	    platform_spec=                                           ;; # ERROR: older than anything we have
    	ia32/cc3.2[._]* )
    	    platform_spec=                                           ;; # ERROR: older than anything we have
    	ia32/cc3.3[._]* )
    	    platform_spec=                                           ;; # ERROR: older than anything we have
    	ia32/cc3.4[._]* )
    	    def_platform_spec_ia32="cc3.4.4_libc2.3.4_kernel2.6.9"   ;; # EL4
    	ia32/cc3.[5-9][._]* | ia32/cc3.[1-9][0-9][._]* )
    	    def_platform_spec_ia32="cc3.4.4_libc2.3.4_kernel2.6.9"   ;; # try the newest cc 3.x we have (EL4)
    	ia32/cc4.[01][._]*_libc2.3[._]* )
    	    def_platform_spec_ia32="cc3.4.4_libc2.3.4_kernel2.6.9"   ;; # Debian with libc 2.3.x
	ia32/cc4.[01][._]* )
    	    def_platform_spec_ia32="cc4.1.0_libc2.4_kernel2.6.16.21" ;; # SLES10
        ia32/* )
            def_platform_spec_ia32="cc4.1.0_libc2.4_kernel2.6.16.21" ;; # default: try the newest cc we have (SLES10)

        intel64/cc3.[01][._]* | intel64/cc[012]* )
	    platform_spec=                                            ;; # ERROR: older than anything we have
        intel64/cc3.2[._]* )
    	    platform_spec=                                            ;; # ERROR: older than anything we have
        intel64/cc3.3[._]* )
    	    platform_spec=                                            ;; # ERROR: older than anything we have
        intel64/cc3.4[._]* )
    	    def_platform_spec_ia32="cc3.4.4_libc2.3.4_kernel2.6.9"   
	    def_platform_spec_intel64="cc3.4.3_libc2.3.4_kernel2.6.9"   ;; # EL4
        intel64/cc3.[5-9][._]* | intel64/cc3.[1-9][0-9][._]* )
    	    def_platform_spec_ia32="cc3.4.4_libc2.3.4_kernel2.6.9"   
            def_platform_spec_intel64="cc3.4.3_libc2.3.4_kernel2.6.9"   ;; # try the newest cc 3.x we have (EL4)
        intel64/cc4.[01][._]*_libc2.3[._]* )
    	    def_platform_spec_ia32="cc3.4.4_libc2.3.4_kernel2.6.9"   
	    def_platform_spec_intel64="cc3.4.3_libc2.3.4_kernel2.6.9"   ;; # Debian with libc 2.3.x
        intel64/cc4.[01][._]* )
    	    def_platform_spec_ia32="cc4.1.0_libc2.4_kernel2.6.16.21" 
            def_platform_spec_intel64="cc4.1.0_libc2.4_kernel2.6.16.21" ;; # SLES10
        intel64/* )
            def_platform_spec_ia32="cc4.1.0_libc2.4_kernel2.6.16.21"  
            def_platform_spec_intel64="cc4.1.0_libc2.4_kernel2.6.16.21" ;; # default: try the newest cc we have (SLES10)														    
        * ) 
	    platform_spec= ;;
    esac
    
    case "$processor" in
	ia32)
    	    def_platform_spec=$def_platform_spec_ia32;;
	ia64)
	    def_platform_spec=$def_platform_spec_ia64;;
        intel64)
    	    def_platform_spec=$def_platform_spec_intel64;;
    esac

    if [ -n "$def_platform_spec" ]; then
	platform_spec="$def_platform_spec"
    elif [ -z "$platform_spec" ]; then
	check_for_valid_platform_warning "unrecognized platform: ${processor}/${orig_platform_spec}"
	if [ "$RPM_YES_NO" == "1" ]; then
	    check_for_valid_platform_warning "unrecognized platform: ${processor}/${orig_platform_spec}" >> $temp_file
	fi
    fi
}

check_for_valid_platform()
{
    platform_spec=

    orig_platform_spec=

    def_platform_spec=
    def_platform_spec_ia32=
    def_platform_spec_ia64=
    def_platform_spec_intel64=

    had_check_platform_warnings=

    my_uname=`uname`
    case "$my_uname" in
	Linux )        platform="linux" ;;
	* ) check_for_valid_platform_error "unsupported platform: uname=$my_uname" ;;
    esac

    check_for_valid_platform_$platform
}


################################################

remove_symlinks()
{
    rm -f $INSTALL_DIR/bin/$processor
    rm -f $INSTALL_DIR/lib/$processor
    case "$processor" in
        intel64)
    	    rm -f $INSTALL_DIR/bin/ia32
    	    rm -f $INSTALL_DIR/lib/ia32
	    ;;
    esac
}

create_symlinks()
{
    remove_symlinks

    if [ -n "$def_platform_spec" ]; then
	if [ ! -e $INSTALL_DIR/lib ]; then
	    mkdir $INSTALL_DIR/lib
	fi
	ln -s $INSTALL_DIR/${processor}/${def_platform_spec}/bin $INSTALL_DIR/bin/$processor
	ln -s $INSTALL_DIR/${processor}/${def_platform_spec}/lib $INSTALL_DIR/lib/$processor
    fi
    if [ -n "$def_platform_spec_ia32" ]; then
	case "$processor" in
    	    intel64)
		ln -s $INSTALL_DIR/ia32/${def_platform_spec_ia32}/bin $INSTALL_DIR/bin/ia32
		ln -s $INSTALL_DIR/ia32/${def_platform_spec_ia32}/lib $INSTALL_DIR/lib/ia32 
		;;
	esac
    fi
}

###########################################################

if [ "$RPM_INSTALL_PREFIX" = "" ] ; then
    RPM_INSTALL_PREFIX=$PWD/tbbforexamples
fi

INSTALL_DIR=$RPM_INSTALL_PREFIX

check_for_valid_platform

RETCODE=$?

if [ $RETCODE -eq 0 ]; then
    create_symlinks
fi

#CONFIG_SH=tbvars.sh
#CONFIG_CSH=tbvars.csh
#    
#find $INSTALL_DIR -type f -name tbbvars.*sh -exec /bin/bash -c "\
#    FILE={}; \
#    mv \$FILE \${FILE}.tmp; \
#    sed -e "s:SUBSTITUTE_INSTALL_DIR_HERE:${INSTALL_DIR}:g" \${FILE}.tmp  > \$FILE; \
#    rm -f \${FILE}.tmp" \;
#
#    
#%preun
#!/bin/sh
#
#
# Copyright 2005-2010 Intel Corporation.  All Rights Reserved. 
#
# The source code contained or described herein and all documents related
# to the source code ("Material") are owned by Intel Corporation or its
# suppliers or licensors.  Title to the Material remains with Intel
# Corporation or its suppliers and licensors.  The Material is protected
# by worldwide copyright laws and treaty provisions.  No part of the
# Material may be used, copied, reproduced, modified, published, uploaded,
# posted, transmitted, distributed, or disclosed in any way without
# Intel's prior express written permission.
#
# No license under any patent, copyright, trade secret or other
# intellectual property right is granted to or conferred upon you by
# disclosure or delivery of the Materials, either expressly, by
# implication, inducement, estoppel or otherwise.  Any license under such
# intellectual property rights must be express and approved by Intel in
# writing.
#remove_symlinks()
#{
#    my_arch=`uname -m`
#    case "$my_arch" in
#	i386 | i686 )  processor="ia32" ;;
#	x86_64 )       processor="intel64" ;;
#	ia64 )         processor="ia64" ;;
#    esac
#
#    [ -L "$INSTALL_DIR" ] && destination_dir=`readlink $INSTALL_DIR`
#
#    rm -f $INSTALL_DIR/bin/$processor
#    rm -f $INSTALL_DIR/lib/$processor
#
#    if [ "x$destination_dir" != "x" ]; then
#        if [ -L "$destination_dir/bin/$processor" ]; then
#	    rm -rf $destination_dir/bin/$processor
#	fi
#        if [ -e "$destination_dir/lib" ]; then 
#	    rm -rf $destination_dir/lib
#	fi
#    fi
#
#    if [ "$processor" == "intel64" ]; then
#	rm -f $INSTALL_DIR/bin/ia32
#	rm -f $INSTALL_DIR/lib/ia32
#
#	if [ "x$destination_dir" != "x" ] && [ -L "$destination_dir/bin/ia32" ]; then
#	    rm -rf $destination_dir/bin/ia32
#	fi
#    fi
#    
#    if [ -e "$INSTALL_DIR/lib" ] && [ "`ls $INSTALL_DIR/lib`" == "" ]; then 
#	rm -rf $INSTALL_DIR/lib
#    fi
#}
#
#if [ "$RPM_INSTALL_PREFIX" == "" ] ; then
#    RPM_INSTALL_PREFIX=<!installdir>
#fi
#
#INSTALL_DIR=$RPM_INSTALL_PREFIX
#remove_symlinks
#%files 
#    %defattr(-,root,root)
#    <!installdir>
#    %docdir <!installdir>/doc
#    
#
