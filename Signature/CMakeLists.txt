# CMakeList.txt: проект CMake для Signature; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

include_directories("C:\\libs\\boost")

# Добавьте источник в исполняемый файл этого проекта.
add_executable (Signature "Signature.cpp" "Signature.h" "SignatureMaker.cpp" "Configurations.h" "SignatureTask.h" "Configurations.cpp" "SignatureTask.cpp")

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
