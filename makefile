name=exxx

directory_output=output

directory_cer0=../cer0
directory_cer0_include=${directory_cer0}/include
directory_cer0_library=${directory_cer0}/library/macos/release

directory_clic3=../clic3
directory_clic3_include=${directory_clic3}/include
directory_clic3_library=${directory_clic3}/library/macos/release

directory_math_c=../math_c
directory_math_c_include=${directory_math_c}/include
directory_math_c_library=${directory_math_c}/library/macos/release

file_output=${directory_output}/${name}

file_cer0_library=${directory_cer0_library}/cer0.o
file_clic3_library=${directory_clic3_library}/clic3.o
file_math_c_library=${directory_math_c_library}/math_c.o

ifndef target_device_version
target_device_version=26.1
endif

target_platform=arm64-apple-macos${target_device_version}

directory_sdk=${shell xcrun --sdk macosx${target_device_version} --show-sdk-path}

c_flags_platform=-target ${target_platform} -isysroot ${directory_sdk}

exxx: sources/*.c
	mkdir -p ${directory_output}
	clang ${c_flags_platform} -Iinclude -I${directory_cer0_include} -I${directory_clic3_include} -I${directory_math_c_include} sources/*.c ${file_cer0_library} ${file_clic3_library} ${file_math_c_library} -framework CoreAudio -o ${file_output}

run: .always
	./${file_output}

clean: clean_output

clean_output:
	if [[ -d ${directory_output} ]]; then rm -r ${directory_output}; fi

.always:
