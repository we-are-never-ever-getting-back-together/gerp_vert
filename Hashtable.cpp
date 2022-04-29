#include "Hashtable.h"

void HashTable::processFile(std::string filename)
{
  std::ifstream infile(filename);
  if (infile.is_open())
  {
    files.mutex.lock();
    int fileIdx = files.data.size();
    files.data.push_back(FileData(filename));
    files.mutex.unlock();

    std::string line;
    int line_count = 0;
    while (std::getline(infile, line))
    {
      processLine(line, line_count, fileIdx);
      line_count++;
    }
  }
}

char asciitolower(char in)
{
  if (in <= 'Z' && in >= 'A')
    return in - ('Z' - 'z');
  return in;
}

std::string to_lower(std::string data)
{
  std::transform(data.begin(), data.end(), data.begin(), asciitolower);
  return data;
}

void tokenize(std::string const &str, const char delim,
              std::vector<std::string> &out)
{
  size_t start;
  size_t end = 0;

  while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
  {
    end = str.find(delim, start);
    out.push_back(str.substr(start, end - start));
  }
}

void HashTable::processLine(std::string line, int lineNum, int fileIdx)
{
  // std::istringstream iss(line);
  // std::string word;
  // files.data[fileIdx].mutex.lock();
  // int lineIdx = files.data[fileIdx].lines.size();
  // files.data[fileIdx].lines.push_back(line);
  // files.data[fileIdx].mutex.unlock();
  // Location location(lineIdx, lineNum, fileIdx);
  // while (iss >> word)
  // {

  //   insert(word, location);
  // }

  std::vector<std::string> out;
  tokenize(line, ' ', out);

  files.data[fileIdx].mutex.lock();
  int lineIdx = files.data[fileIdx].lines.size();
  files.data[fileIdx].lines.push_back(line);
  files.data[fileIdx].mutex.unlock();
  Location location(lineIdx, lineNum, fileIdx);
  for (auto &word : out)
  {
    insert(word, location);
  }
}

void HashTable::insert(std::string word, Location loc)
{
  std::string processedWord = to_lower(word);
  int hashvalue = std::hash<std::string>{}(word);
  int idx = (hashvalue % hashtableSize + hashtableSize) % hashtableSize;

  std::atomic<bool> new_baseWord(false);
  for (auto &baseEntry : hashtable[idx].based)
  {
    if (baseEntry.identifier != processedWord)
      continue;
    std::unique_lock<std::timed_mutex> lock(baseEntry.mutex);
    for (auto &entry : baseEntry.entries)
    {
      if (entry.word == word)
      {
        entry.locations.emplace_back(loc);
        new_baseWord = true;
        break;
      }
    }

    break;
  }

  if (new_baseWord)
  {
    std::unique_lock<std::timed_mutex> lock(hashtable[idx].mutex);
    baseWordCount++;
    hashtable[idx].based.emplace_back(BaseEntry(processedWord, Entry(word, loc)));
  }

  // if (baseWordCount / hashtableSize > RATIO)
  // {
  //   resize();
  // }
}

void HashTable::resize()
{
  // int newSize = hashtableSize * RESIZE;
  // HT newtable = HT(newSize);

  // for (auto &values : hashtable)
  // {
  //   for (auto &baseEntry : values.based)
  //   {
  //     int idx = std::hash<std::string>{}(baseEntry.identifier) % newSize;
  //     newtable[idx].push_back(baseEntry);
  //   }
  // }

  // hashtableSize = newSize;
  // hashtable = newtable;
}

std::vector<Location> HashTable::query(std::string word, bool insensitive)
{
  std::vector<Location> res;
  std::string processedWord = to_lower(word);
  int hashvalue = std::hash<std::string>{}(word);
  int idx = (hashvalue % hashtableSize + hashtableSize) % hashtableSize;

  for (auto &baseEntry : hashtable[idx].based)
  {
    if (baseEntry.identifier != processedWord)
      continue;
    for (auto &entry : baseEntry.entries)
    {
      if (insensitive)
      {
        res.insert(res.end(), entry.locations.begin(), entry.locations.end());
      }
      else
      {
        return entry.locations;
      }
    }
  }
  return res;
}

void HashTable::printQueryData(std::string word, std::ostream &outfile, bool insensitive)
{
  auto res = query(word, insensitive);
  for (auto location : res)
  {
    printLine(location, outfile);
  }
}

void HashTable::printLine(Location location, std::ostream &outfile)
{
  // outfile << filenames[location.file_idx] << ":" << location.line_infile << ": " << lines[location.line_idx];
}