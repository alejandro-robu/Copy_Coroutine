
// Este código es de dominio público
// angel.rodriguez@udit.es

#include <iostream>
#include <string>
#include <thread>

#include "Resumable.hpp"
#include <fstream>
#include <filesystem>
#include <assert.h>
#include <list>

using namespace std;
using namespace udit;

namespace udit
{
    Resumable resumable_copy (const filesystem::path input_file_path, const filesystem::path output_file_path, std::size_t chunk_size = 64)
    {
        ifstream reader(input_file_path, ios::binary);
        if (!reader) co_return;

        ofstream writer(output_file_path, ios::binary | ios::trunc);
        if (!writer) co_return;

        std::vector<char> buffer(chunk_size);

        while (not reader.eof())
        {
            reader.read(buffer.data(), buffer.size());
            auto bytes = reader.gcount();

            if (bytes > 0) 
            {
                writer.write(buffer.data(), bytes);
                co_await std::suspend_always{};
            }
        };
    }

    void use_copy_coroutine (const filesystem::path& input, const filesystem::path& output, const int max_coroutines = 4)
    {
        std::list<Resumable> left_coroutines;
        std::list<Resumable> coroutines;

        filesystem::directory_iterator dir_iterator{ input };

        //int current_coroutine = max_coroutines;

        int number_of_coroutines = 0;
        for (auto const& dir_entry : dir_iterator)
        {
            if (!dir_entry.is_regular_file())
                continue;

            filesystem::path out_file = output / dir_entry.path().filename();


            if (number_of_coroutines < max_coroutines) 
            {
                coroutines.emplace_back(resumable_copy(dir_entry.path(), out_file, 64));
            }
            else
            {
                left_coroutines.emplace_back(resumable_copy(dir_entry.path(), out_file, 64));
            }
            number_of_coroutines++;

        }

        while (coroutines.size() > 0)
        {
            for (auto coroutine = coroutines.begin(); coroutine != coroutines.end(); )
            {
                if (!coroutine->done())
                {
                    cout << "I am working " << endl;
                    coroutine->resume();
                    ++coroutine;
                }
                else
                {
                    cout << "I am done " << endl;
                    coroutine = coroutines.erase(coroutine);

                    if (left_coroutines.size() > 0)
                    {
                        //for (size_t i = 0; i < current_coroutine; i++)
                        //{
                        //    dir_iterator.increment();
                        //    coroutines.emplace_back(resumable_copy(dir_entry.path(), out_file, 64));
                        // 
                        //}
                        cout << "ADDED FROM LEFT_COROUTINES" << endl;
                        auto top_coroutine = left_coroutines.begin();
                        coroutines.emplace_back(move(*top_coroutine));
                        left_coroutines.erase(top_coroutine);



                    }
                }
            }
        }
        //while (work_remaining) 
        //{ 
        //    work_remaining = false; 
        //    int count = 0; 
        //    for (auto& task : tasks) 
        //    { 
        //        if (!task.done()) 
        //        { 
        //            cout << "I am working " << count << endl; 
        //            task.resume(); 
        //            work_remaining = true; 
        //        } 
        //        else 
        //        { 
        //            cout << "I am done " << count << endl; 
        //            //tasks.remove(task); 
        //        } 
        //        count++; 
        //    } 
        //}
    }



    //bool copy(const std::filesystem::path input_file_path, const std::filesystem::path  output_file_path)
    //{
    //    ifstream reader(input_file_path, ifstream::binary);

    //    if (!reader) return false;

    //    ofstream writer(output_file_path, ifstream::binary | ifstream::trunc);

    //    if (!writer) return false;

    //    vector<char> buffer(65536u);

    //    do
    //    {
    //        reader.read(buffer.data(), buffer.size());

    //        // comprobar errores

    //        auto bytes_written = reader.gcount();

    //        writer.write(buffer.data(), bytes_written);

    //        // comprobar posibles errores

    //    } while (not reader.eof());
    //}
}

//int main()
//{
//    std::string input_folder = "../../Assets/Input";
//    std::string output_folder = "../../Assets/Output";
//
//    const std::filesystem::path input{ input_folder };
//    const std::filesystem::path output{ output_folder };
//
//
//    if (!std::filesystem::exists(input))
//    {
//        std::cout << "No existe la carpeta de los inputs\n";
//        return 1;
//    }
//
//    if (!std::filesystem::exists(output))
//    {
//        std::cout << "No existe la carpeta de outputs, la creo.\n";
//        std::filesystem::create_directory(output_folder);
//    }
//
//
//    //QUITA TODO EL OUTPUT
//    for (auto const& dir_entry : std::filesystem::directory_iterator{ output })
//    {
//        std::filesystem::remove(dir_entry.path());
//    }
//
//    //AÑADE AL OUTPUT EL INPUT
//    for (auto const& dir_entry : std::filesystem::directory_iterator{ input })
//    {
//        //std::cout << dir_entry.path() << '\n';
//        std::filesystem::path output_file = output_folder / dir_entry.path().filename();
//        //udit::copy(dir_entry.path(), output_file);
//        udit::copy(dir_entry.path(), output_file);
//    }
//
//    //// delete the sandbox dir and all contents within it, including subdirs
//    //for (auto const& dir_entry : std::filesystem::directory_iterator{ input })
//    //{
//    //    std::filesystem::remove(dir_entry.path());
//    //}
//
//
//}


int main()
{
    std::string input_folder = "../../Assets/Input";
    std::string output_folder = "../../Assets/Output";

    const std::filesystem::path input{ input_folder };
    const std::filesystem::path output{ output_folder };


    if (!std::filesystem::exists(input))
    {
        std::cout << "No existe la carpeta de los inputs\n";
        return 1;
    }

    if (!std::filesystem::exists(output))
    {
        std::cout << "No existe la carpeta de outputs, la creo.\n";
        std::filesystem::create_directory(output_folder);
    }

    //QUITA TODO EL OUTPUT
    for (auto const& dir_entry : std::filesystem::directory_iterator{ output })
    {
        std::filesystem::remove(dir_entry.path());
    }

    use_copy_coroutine(input, output, 4);
}