cmake_minimum_required(VERSION 3.26)
project(HamArc)

set(CMAKE_CXX_STANDARD 17)

add_executable(HamArc main.cpp
        HamArc.h
        HamArc.cpp
        TArgumentParser.h
        TArgumentParser.cpp
        HammingWriter.h
        HammingWriter.cpp
        HammingReader.h
        HammingReader.cpp
)
