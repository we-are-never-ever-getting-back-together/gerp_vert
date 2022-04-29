#include <iostream>
#include <stdlib.h>
#include <fstream>
#include "Hashtable.h"
#include <thread>
#include <filesystem>
#include <chrono>
namespace fsys = std::__fs::filesystem;

using namespace std::chrono;

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    std::cerr << "Usage : ./gerp inputDirectory outputFile\n";
    return EXIT_FAILURE;
  }

  // Traverse Directories and insert into query_table;
  std::string inputPath = argv[1];
  auto start = high_resolution_clock::now();

  // concurrency test
  std::vector<std::thread> threads;
  int filecount = 0;
  for (const auto &entry : fsys::recursive_directory_iterator(inputPath))
  {
    if (entry.path().extension() != ".txt")
      continue;
    filecount++;
  }
  HashTable query_table(filecount);
  for (const auto &entry : fsys::recursive_directory_iterator(inputPath))
  {
    if (entry.path().extension() != ".txt")
      continue;
    std::string path = entry.path();
    std::thread t([qt = &query_table, path]()
                  { qt->processFile(path); });
    threads.push_back(std::move(t));
  }

  for (auto &t : threads)
  {
    if (t.joinable())
    {
      t.join();
    }
  }

  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  std::cout << "Time taken by function: "
            << duration.count() << " microseconds" << std::endl;

  return 0;
}

// void execution_loop(HashTable &query_table, std::string outfile_path)
// {
//   std::string query;

//   while (true)
//   {
//     std::cout << "Query? ";
//     std::cin >> query;
//     if (query == "@quit" || "@q")
//     {
//       std::cout << "Goodbye! Thank you and have a nice day." << std::endl;
//     }
//     else if (is_filechange(query))
//     {
//       auto new_file_path = get_second_word(query);
//     }
//     else if (is_insensitive(query))
//     {
//       auto sanitized_word = get_second_word(query);
//     }
//     else
//     {
//       auto sanitized_word = get_second_word(query);
//     }
//   }
// }

// bool is_insensitive(std::string query)
// {
//   std::string insensitive_prefix_shorthand = "@i";
//   std::string insensitive_prefix = "@insenstive";
//   auto match_long = std::mismatch(insensitive_prefix.begin(), insensitive_prefix.end(), query.begin());
//   auto match_short = std::mismatch(insensitive_prefix_shorthand.begin(), insensitive_prefix_shorthand.end(), query.begin());

//   return (match_long.first == insensitive_prefix.end()) || (match_short.first == insensitive_prefix_shorthand.end());
// }

// bool is_filechange(std::string query)
// {

//   std::string filechange_prefix = "@f";
//   auto match_short = std::mismatch(filechange_prefix.begin(), filechange_prefix.end(), query.begin());
//   return match_short.first == filechange_prefix.end();
// }

// std::string get_second_word(std::string query)
// {
//   size_t pos1 = query.find(' ');
//   size_t pos2 = query.find(' ', pos1 + 1);
//   return query.substr(pos1 + 1, pos2 - pos1);
// }