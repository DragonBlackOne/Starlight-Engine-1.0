if (NOT EXISTS "D:/Projetos/Fusion ENGINE/TitanEngine/build_final_test/install_manifest.txt")
    message(FATAL_ERROR "Cannot find install manifest: \"D:/Projetos/Fusion ENGINE/TitanEngine/build_final_test/install_manifest.txt\"")
endif(NOT EXISTS "D:/Projetos/Fusion ENGINE/TitanEngine/build_final_test/install_manifest.txt")

file(READ "D:/Projetos/Fusion ENGINE/TitanEngine/build_final_test/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")
foreach (file ${files})
    message(STATUS "Uninstalling \"$ENV{DESTDIR}${file}\"")
    execute_process(
        COMMAND C:/Users/alyss/AppData/Local/Programs/Python/Python313/Lib/site-packages/cmake/data/bin/cmake.exe -E remove "$ENV{DESTDIR}${file}"
        OUTPUT_VARIABLE rm_out
        RESULT_VARIABLE rm_retval
    )
    if(NOT ${rm_retval} EQUAL 0)
        message(FATAL_ERROR "Problem when removing \"$ENV{DESTDIR}${file}\"")
    endif (NOT ${rm_retval} EQUAL 0)
endforeach(file)

