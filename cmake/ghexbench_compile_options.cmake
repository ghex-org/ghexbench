
set(c_cxx_lang "$<COMPILE_LANGUAGE:C,CXX>")
set(c_cxx_lang_clang "$<COMPILE_LANG_AND_ID:CXX,Clang>")

function(ghexbench_target_compile_options target)
    target_compile_options(${target} PRIVATE
    # flags for CXX builds
    $<${c_cxx_lang}:$<BUILD_INTERFACE:-Wall -Wextra -Wpedantic -Wno-unknown-pragmas>>
    $<${c_cxx_lang_clang}:$<BUILD_INTERFACE:-Wno-c++17-extensions -Wno-unused-lambda-capture>>
    )
endfunction()
