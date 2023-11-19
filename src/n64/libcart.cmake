message("Configuring libcart")

# Note When upgrading: If a file has a .c and .s version then then .s should be used for libultra and .c used for libdragon

set(libcart_dir ${CMAKE_CURRENT_SOURCE_DIR}/n64/libcart)
set(libcart_include_dir ${libcart_dir}/include)
set(libcart_src_dir ${libcart_dir}/src)
set(libcart_ff_dir ${libcart_dir}/ff)

set(libcart_include_file ${libcart_include_dir}/cart.h)

set(libcart_src_files
    ${libcart_src_dir}/cart.c
    ${libcart_src_dir}/cartcardinit.c
    ${libcart_src_dir}/cartcardrdcart.c
    ${libcart_src_dir}/cartcardrddram.c
    ${libcart_src_dir}/cartcardwrcart.c
    ${libcart_src_dir}/cartcardwrdram.c
    ${libcart_src_dir}/cartexit.c
    ${libcart_src_dir}/cartinit.c
    ${libcart_src_dir}/cartint.h
    ${libcart_src_dir}/ci.c
    ${libcart_src_dir}/ci.h
    ${libcart_src_dir}/cicardinit.c
    ${libcart_src_dir}/cicardrdcart.c
    ${libcart_src_dir}/cicardrddram.c
    ${libcart_src_dir}/cicardwrcart.c
    ${libcart_src_dir}/cicardwrdram.c
    ${libcart_src_dir}/ciexit.c
    ${libcart_src_dir}/ciinit.c
    ${libcart_src_dir}/ed.h
    ${libcart_src_dir}/edcard.c
    ${libcart_src_dir}/edcardinit.c
    ${libcart_src_dir}/edcardrdcart.c
    ${libcart_src_dir}/edcardrddram.c
    ${libcart_src_dir}/edcardwrcart.c
    ${libcart_src_dir}/edcardwrdram.c
    ${libcart_src_dir}/edexit.c
    ${libcart_src_dir}/edinit.c
    ${libcart_src_dir}/edx.h
    ${libcart_src_dir}/edxcard.c
    ${libcart_src_dir}/edxcardinit.c
    ${libcart_src_dir}/edxcardrdcart.c
    ${libcart_src_dir}/edxcardrddram.c
    ${libcart_src_dir}/edxcardwrcart.c
    ${libcart_src_dir}/edxcardwrdram.c
    ${libcart_src_dir}/edxexit.c
    ${libcart_src_dir}/edxinit.c
    ${libcart_src_dir}/sc.c
    ${libcart_src_dir}/sc.h
    ${libcart_src_dir}/sccardinit.c
    ${libcart_src_dir}/sccardrdcart.c
    ${libcart_src_dir}/sccardrddram.c
    ${libcart_src_dir}/sccardwrcart.c
    ${libcart_src_dir}/sccardwrdram.c
    ${libcart_src_dir}/scexit.c
    ${libcart_src_dir}/scinit.c
    ${libcart_src_dir}/sd.c
    ${libcart_src_dir}/sd.h
)

set(libcart_assembly_files 
    ${libcart_src_dir}/cartbuf.s
    ${libcart_src_dir}/sdcrc16.s
)

set(ff_src_files
    ${libcart_ff_dir}/diskio.c
    ${libcart_ff_dir}/diskio.h
    ${libcart_ff_dir}/ff.c
    ${libcart_ff_dir}/ff.h
    ${libcart_ff_dir}/ffconf.h
    ${libcart_ff_dir}/ffsystem.c
    ${libcart_ff_dir}/ffunicode.c
)

target_compile_definitions(framework64 PRIVATE -D_ULTRA64)
target_include_directories(framework64 PRIVATE ${libcart_include_dir} ${libcart_ff_dir})
target_sources(framework64 PRIVATE 
    ${libcart_include_file}
    ${libcart_src_files}
    ${libcart_assembly_files}
    ${ff_src_files})

set(asm_compile_options "-x;assembler-with-cpp;-MMD")
set_source_files_properties(${libcart_src_dir}/cartbuf.s PROPERTIES LANGUAGE C COMPILE_OPTIONS "${asm_compile_options}")
set_source_files_properties(${libcart_src_dir}/sdcrc16.s PROPERTIES LANGUAGE C COMPILE_OPTIONS "${asm_compile_options}")