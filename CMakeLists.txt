cmake_minimum_required(VERSION 3.25)
project(dz_sem_2 C)

set(CMAKE_C_STANDARD 11)

add_executable(${PROJECT_NAME}
        common.c
        main.c
        dynamic_array.c
        csv/csv_row.c
        csv/csv_parser.c
        books.c
        students.c
        users.c
        )
