#pragma once

#include <string>
#include <vector>
#include <algorithm>

namespace kantu {

std::string to_lower(const std::string& str);
std::string& replace_all(std::string& src, const std::string& old_value, const std::string& new_value);


class FilePath
{
public:
    FilePath(const std::string filepath) :
        m_path(filepath),
        m_directory(""),
        m_filename(""),
        m_basename(""),
        m_ext("")
    {
        get_directory_and_filename();
        get_basename_and_ext();
    }

private:
    void get_directory_and_filename()
    {
        int pos = (int)m_path.length() - 1;
        for (; pos >= 0; pos--)
        {
            if (m_path[pos] == '/' || m_path[pos] == '\\')
            {
                break;
            }
        }
        if (pos == -1)
        {
            m_filename = m_path;
        }
        else
        {
            m_directory = m_path.substr(0, pos);
            m_filename = m_path.substr(pos + 1);
        }
    }

    void get_basename_and_ext()
    {
        std::string::size_type pos = m_filename.find_last_of('.');
        if (pos == std::string::npos)
        {
            m_basename = m_filename;
        }
        else
        {
            m_basename = m_filename.substr(0, pos);
            m_ext = m_filename.substr(pos + 1);
        }
    }

public:
    std::string path() const
    {
        return m_path;
    }

    std::string directory() const
    {
        return m_directory;
    }

    std::string filename() const
    {
        return m_filename;
    }

    std::string basename() const
    {
        return m_basename;
    }

    std::string ext() const
    {
        return m_ext;
    }

private:
    // path = [directory/]filename
    std::string m_path;
    std::string m_directory;

    // filename = basename.ext
    std::string m_filename;

    std::string m_basename;
    std::string m_ext;
};

template<typename T>
int find(const std::vector<T>& vec, const T& target)
{
    int pos = -1;
    typename std::vector<T>::const_iterator iter = std::find(vec.begin(), vec.end(), target);
    if (iter != vec.end())
    {
        pos = (int)std::distance(vec.begin(), iter);
    }
    return pos;
}

} // namespace kantu