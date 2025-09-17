name=exxx

directory_assets=assets
directory_assets_cho=${directory_assets}/cho
directory_output=output

directory_cer0=../cer0
directory_cer0_include=${directory_cer0}/include
directory_cer0_library=${directory_cer0}/library

directory_clic3=../clic3
directory_clic3_include=${directory_clic3}/include
directory_clic3_library=${directory_clic3}/library

file_output=${directory_output}/${name}

file_cer0_library=${directory_cer0_library}/cer0.o
file_clic3_library=${directory_clic3_library}/clic3.o

prefix_asset=https://content.alic2.dev/7f0205eb-66d5-41dc-8952-bbeb765d0bb3

all:
	mkdir -p ${directory_output}
	-ln -s ${shell pwd}/assets -T ${directory_output}/assets
	clang -Iinclude -I${directory_cer0_include} -I${directory_clic3_include} sources/*.c ${file_cer0_library} ${file_clic3_library} -framework CoreAudio -o ${file_output}

run: .always
	./${file_output}

pull_assets: .always
	mkdir -p ${directory_assets}
	mkdir -p ${directory_assets_cho}
	curl ${prefix_asset}/506dd432-4b1d-4cda-a757-2aba6c3885f4.json -o ${directory_assets_cho}/municipal_boundary_area.json
	curl ${prefix_asset}/12e6fbd4-3965-4c4a-a060-bbfa64655c26.json -o ${directory_assets_cho}/road_area.json
	curl ${prefix_asset}/566537a6-ed50-4a70-bbae-233aa0cae5a6.json -o ${directory_assets_cho}/road_bridge_area.json
	curl ${prefix_asset}/87efe2f2-91a9-49ae-9190-ce3687ec4f94.json -o ${directory_assets_cho}/structure_existing_area.json
	curl ${prefix_asset}/293dd87e-9588-4dab-a928-4ff552718111.json -o ${directory_assets_cho}/surface_water_course_area.json
	curl ${prefix_asset}/7fde410a-6af8-4692-90d0-b611a782f8d7.json -o ${directory_assets_cho}/railroad_centerline.json
	curl ${prefix_asset}/1b9688a3-4344-43cd-812d-2c3b5f558e4d.json -o ${directory_assets_cho}/park_area.json
	curl ${prefix_asset}/3402f81f-2d0e-48d2-88e5-58034afe4913.json -o ${directory_assets_cho}/pedestrian_sidewalk_area.json
	curl ${prefix_asset}/b2471620-b02a-428b-b6cb-617536a8212a.json -o ${directory_assets_cho}/pedestrian_sidewalk_bridge_area.json

.always:
