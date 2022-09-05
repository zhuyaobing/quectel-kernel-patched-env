/*
 *******************************************************************************
 *******************************************************************************
 ***
 ***     File: filesystem.h
 ***
 ***     Project: Alexandra
 ***
 ***     Copyright (C) Stoneridge Electronics AB 1990 - 2019
 ***
 *******************************************************************************
 */


/*
 **=============================================================================
 ** Header definition
 **=============================================================================
 */
#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

/*
 **=============================================================================
 ** Imported definitions
 **=============================================================================
 */
#include <string>
#include <memory>
#include <unistd.h>

/*
 **=============================================================================
 ** Public type definitions
 **=============================================================================
 */
namespace ecu {
namespace lapi {
/**
 *******************************************************************************
 *
 *  @brief Contains file system functionalities
 *
 *******************************************************************************
 */
namespace fs {

/**
 *******************************************************************************
 *
 * @brief Supporing class to create a temporary directory.
 *
 * @details Usually used to unit tests to verification of functionalities.
 *
 * @note The path will be deleted once the instance is destroyed.
 *
 * Example Creating a directory and checking if was created
 * @code
 *   auto tmpdir = ecu::lapi::TempDirectory::create("/tmp");
 *   auto path = tmpdir->path();
 *   std::string category("category");
 *
 *   ASSERT_FALSE(ecu::lapi::fs::exists(path + "/" + category));
 *
 *   if(ecu::lapi::mkdir(path + "/" + category))
 *   {
 *      ASSERT_TRUE(ecu::lapi::fs::exists(path + "/" + category));
 *   }
 * @endcode
 *
 *******************************************************************************
 */
class TempDirectory
{
public:
   /**
   ****************************************************************************
   *
   * Convenience "using" to a unique pointer type of this class.
   *
   ****************************************************************************
   */
   using Ptr = std::unique_ptr<const TempDirectory>;

   /**
   ****************************************************************************
   *
   * @brief Create a temp directory.
   *
   * @param base_dir Path to file.
   *
   * @return Return a unique pointer to object created.
   *
   ****************************************************************************
   */
   static Ptr create(const std::string& base_dir);

   /**
   ****************************************************************************
   *
   * @brief Get a path of the file created.
   *
   * @return Return a string where the file is located.
   *
   ****************************************************************************
   */
   const std::string& path() const { return m_temp_dir; }

   /**
   ****************************************************************************
   *
   * Destructor
   *
   ****************************************************************************
   */
   ~TempDirectory();

   /**
   ****************************************************************************
   *
   * Constructor
   *
   ****************************************************************************
   */
   TempDirectory(const TempDirectory&) = delete;

private:
   explicit TempDirectory(const std::string& base_dir);

   void init();
   void destroy();
   std::string m_base;
   std::string m_temp_dir;
};

/**
 ***************************************************************************
 *
 * @brief Check if a file exist.
 *
 * @param path Path to file.
 *
 * @return Return true if the file exist, otherwise false.
 *
 ****************************************************************************
 */
bool exists(const std::string& path);

/**
 ***************************************************************************
 *
 * @brief Check if the path is a directory.
 *
 * @param path Path to the directory candidate.
 *
 * @return Return true if the path is a directory, otherwise false.
 *
 ***************************************************************************
 */
bool is_dir(const std::string& path);

/**
 ***************************************************************************
 *
 * @brief Check if the path is a file.
 *
 * @param path Path to the file candidate.
 *
 * @return Return true if the path is a file, otherwise false.
 *
 ***************************************************************************
 */
bool is_file(const std::string& path);

/**
 ***************************************************************************
 *
 * @brief Checks if the given path is a symbolic link, see man 2 lstat.
 *
 * @param path Path to the symbolic link.
 *
 * @return Return true if the path is a symbolic link, otherwise false.
 *
 ***************************************************************************
 */
bool is_symlink(const std::string& path);

/**
 ***************************************************************************
 *
 * @brief Create a directory.
 *
 * @details it creates the entire directory tree up to the given directory.
 *
 * @param path Name of directory.
 *
 * @return Return true if the file is created successfull, otherwise false.
 *
 ***************************************************************************
 */
bool mkdir(const std::string& path);

/**
 ***************************************************************************
 *
 * @brief Get the size of a given path (file or directory).
 *
 * @details if size could not be taken, -1 is returned.
 *
 * @param path Path to the file or directory candidate.
 *
 * @return the size of the give path in bytes, in case of error -1.
 *
 ***************************************************************************
 */
int64_t
size(const std::string& path);

/**
 ***************************************************************************
 *
 * @brief Truncate a file to give size (length).
 *
 * @details In case of an error, the function will print a error message and
 * returns false!
 *
 * @param file Path to the file.
 *
 * @param size Length to be truncated.
 *
 * @return true if truncation worked, otherwise false, e.g. file doesn't
 * exist or is a directory.
 *
 ***************************************************************************
 */
bool
truncate(const std::string& file, std::size_t size);

/**
 ***************************************************************************
 *
 * @brief Gets the dirname of the given path, see man 3 dirname.
 *
 * @param path Path to the directory.
 *
 * @return The dirname as a string.
 *
 ***************************************************************************
 */
std::string dirname(const std::string& path);

/**
 ***************************************************************************
 *
 * @brief Gets the filename/basename of the given path, see man 3 basename.
 *
 * @note the compiler does not allow use of 'basename' as a function name,
 * it prepends __xpg_ to the function.
 *
 * @param path Path to be checked.
 *
 * @return The basename as a string.
 *
 ***************************************************************************
 */
std::string filename(const std::string& path);

/**
 ***************************************************************************
 *
 * @brief Resolves the path fully, see man 3 realpath.
 *
 * @details Will resolve all links and relative elements of path supplied
 * and return an absolute path!
 *
 * @param path Path to resolve.
 *
 * @return The fully resolved absolute path.
 *
 ***************************************************************************
 */
std::string realpath(const std::string& path);

/**
 ***************************************************************************
 *
 * @brief Deletes/remove the given path from the filesystem.
 *
 * @param path Path to be deleted.
 *
 * @param delete_base true if you want to delete base file, otherwise false.
 *
 * @return true if files have been remove correctly, otherwise false.
 *
 ***************************************************************************
 */
bool remove(const std::string& path, bool delete_base=true);

/**
 ****************************************************************************
 * @brief Read a file's contents into a std::string
 *
 * @param file File name
 *
 * @return std::String containing file contents
 ****************************************************************************
 */
std::string
read_file_into_string(const std::string& file);

/**
 ****************************************************************************
 * @brief Create a file containing the characters from a std::string
 *
 * @param file    File name
 * @param content Contents of created file
 *
 * @return The number of bytes written to the file or 0 on error
 ****************************************************************************
 */
size_t
create_file_with_contents(const std::string& file, const std::string& contents);

/**
 ****************************************************************************
 * @brief Create a simple symbolic link
 *
 * @param linked_path Path which will be linked
 * @param link_name   Name of the link which will be created
 *
 * @return Upon successful, return 0. Otherwise, returns -1
 ****************************************************************************
 */
int
symlink(const std::string& linked_path, const std::string& link_name);

/**
 ****************************************************************************
 * @brief Remove symbolic link
 *
 * @param pathname Path to symbolic link which will be deleted
 *
 * @return Upon successful, return 0. Otherwise, returns -1
 ****************************************************************************
 */
int
unlink(const std::string& pathname);

/**
 * @brief Read and return the path to which a symlink links
 *
 * @param[in] symlink_path  The path  to the symlink to read
 *
 * @return the path to which the symlink links
 */
std::string
read_symlink( const std::string& symlink_path );

/**
 ***************************************************************************
 *
 * @brief Move source_file to a new location (destination_path), where
 * destination_path can either be directory or the new file name.
 *
 * @details If source_file is not a regular, the function prints an error
 * and returns with false.
 *
 * @param source_file Source path.
 *
 * @param destination_path Destination path.
 *
 * @return true if move succeeded, otherwise false.
 *
 ***************************************************************************
 */
bool
move(const std::string& source_file, const std::string& destination_path);

/**
 ***************************************************************************
 *
 * @brief Copy source file to a new destination path.
 *
 * @param source Path to the source file.
 *
 * @param destination Destination path.
 *
 * @return true if move succeeded, otherwise false.
 *
 ***************************************************************************
 */
bool
copy_file(const std::string& source, const std::string& destination);

/**
 * @brief Move a directory
 *
 * @param[in] dirname_source  Source directory path
 * @param[in] dirname_dest    Dest directory path
 *
 * @return true on success, otherwise false
 */
bool
move_directory(const std::string& dirname_source, const std::string& dirname_dest);

/**
 * @brief Return a given directory data size in MB. If a path
 * to a regular file or of any other kind, the functions returns
 * false.
 *
 * @param path path to the directory or file.
 * @param size variable to store the size.
 * @return true if successed.
 * @return false if fail.
 */
bool
dir_size(const std::string &path, size_t &size);

}  // namespace fs
}  // namespace lapi
}  // namespace ecu

#endif  // FILESYSTEM_H_
