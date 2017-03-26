/*
 * File: TestFilelibLibrary.cpp
 * ----------------------------
 * This file tests the functions in the filelib.h interface.
 */

/*************************************************************************/
/* Stanford Portable Library                                             */
/* Copyright (c) 2014 by Eric Roberts <eroberts@cs.stanford.edu>         */
/*                                                                       */
/* This program is free software: you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by  */
/* the Free Software Foundation, either version 3 of the License, or     */
/* (at your option) any later version.                                   */
/*                                                                       */
/* This program is distributed in the hope that it will be useful,       */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/* GNU General Public License for more details.                          */
/*                                                                       */
/* You should have received a copy of the GNU General Public License     */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */
/*************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include "filelib.h"
#include "strlib.h"
#include "error.h"
#include "unittest.h"
#ifdef _WIN32
    #define _WIN32_WINNT 0x0A00  // Windows 10
    #include <windows.h>
#endif
using namespace std;

/* Function prototypes */

static void testCreateDirectory(void);
static void testCreateDirectoryPath(void);
static void testFileSize(void);
static void testIsSymbolicLink(void);
static void testSetGetCurrentDirectory(void);
static void testGetRoot(void);
static void testGetExtension(void);
static void testGetHead(void);
static void testGetTail(void);
static void testDefaultExtension(void);
static void testDeleteFile(void);
static void testFileExists(void);
static void testGetTempDirectory(void);
static void testIsFile(void);
static void testIsDirectory(void);
static void testExpandPathname(void);
static void testMatchFilenamePattern(void);

void testFilelibLibrary() {
   testCreateDirectory();
   testCreateDirectoryPath();
   testFileSize();
   testDeleteFile();
   testIsSymbolicLink();
   testSetGetCurrentDirectory();
   testGetRoot();
   testGetExtension();
   testGetHead();
   testGetTail();
   testGetTempDirectory();
   testDefaultExtension();
   testFileExists();
   testIsFile();
   testIsDirectory();
   testExpandPathname();
   testMatchFilenamePattern();
   reportResult("filelib.h library");
}

static void testCreateDirectory() {
    createDirectory("ddd");
    test(isDirectory("ddd"), true);
#ifdef _WIN32
    checkErrorStartsWith(createDirectory("zymurgy/gorp/corfu"), "createDirectory:");
#else
    checkError(createDirectory("zymurgy/gorp/corfu"), "createDirectory: No such file or directory");
#endif
    deleteFile("ddd");
}

static void testFileSize() {
    test(fileSize("non/existent/file"), -1);
    ofstream ofs;
    ofs.open("empty_file_2112");
    test(fileSize("empty_file_2112"), 0);
    ofs.close();
    deleteFile("empty_file_2112");
    ofs.open("file_1234");
    for (int i = 0; i < 1234; i++) {
        ofs << 'X';
    }
    ofs.close();
    test(fileSize("file_1234"), 1234);
    deleteFile("file_1234");

}

static void testCreateDirectoryPath() {
    createDirectoryPath("qqq/rrr/sss");
    test(isDirectory("qqq/rrr/sss"), true);
    deleteFile("qqq/rrr/sss");
    deleteFile("qqq/rrr");
    deleteFile("qqq");

    createDirectory("abcd");
    createDirectoryPath("abcd/efgh");
    test(isDirectory("abcd/efgh"), true);
    createDirectory("abcd/efgh/ijkl");
    test(isDirectory("abcd/efgh/ijkl"), true);
    deleteFile("abcd/efgh/ijkl");
    deleteFile("abcd/efgh");
    deleteFile("abcd");

#ifdef _WIN32
    createDirectoryPath("\\a11b22c33XYZ\\xxx\\yyy");
    test(isDirectory("/a11b22c33XYZ/xxx/yyy"), true);
    deleteFile("\\a11b22c33XYZ\\xxx\\yyy");
    deleteFile("/a11b22c33XYZ/xxx");
    deleteFile("/a11b22c33XYZ");

    createDirectoryPath("c:\\a11b22c33XYZ\\xxx\\yyy");
    test(isDirectory("c:/a11b22c33XYZ/xxx/yyy"), true);
    deleteFile("c:\\a11b22c33XYZ\\xxx\\yyy");
    deleteFile("C:/a11b22c33XYZ/xxx");
    deleteFile("C:/a11b22c33XYZ");
#else
    createDirectoryPath("\\tmp\\xxx\\yyy");
    test(isDirectory("/tmp/xxx/yyy"), true);
    deleteFile("\\tmp\\xxx\\yyy");
    deleteFile("/tmp/xxx");
#endif
    createDirectoryPath("./a1b2c3XYZ");
    test(isDirectory(".\\a1b2c3XYZ"), true);
    deleteFile("./a1b2c3XYZ");
}

static void testSetGetCurrentDirectory() {
    string saveDir = getCurrentDirectory();
    setCurrentDirectory("/");
    cout << "CURRDIR: " << getCurrentDirectory() << endl;
    setCurrentDirectory(saveDir);
    cout << "CURRDIR: " << getCurrentDirectory() << endl;
    test(getCurrentDirectory() == saveDir, true);
}

static void testGetRoot() {
   test(getRoot("filename.ext"), "filename");
   test(getRoot("abc.def.ghi"), "abc.def");
   test(getRoot("/a/b/c/filename.ext"), "/a/b/c/filename");
   test(getRoot("A:\\a\\b\\c\\filename.ext"), "A:\\a\\b\\c\\filename");
   test(getRoot("A:/a/b/c/filename.ext"), "A:/a/b/c/filename");
}

static void testGetExtension() {
   test(getExtension("filename.ext"), ".ext");
   test(getExtension("abc.def.ghi"), ".ghi");
   test(getExtension("/a/b/c/filename.ext"), ".ext");
   test(getExtension("A:\\a\\b\\c\\filename.ext"), ".ext");
   test(getExtension("A:/a/b/c/filename.ext"), ".ext");
}

static void testGetHead() {
   test(getHead("/a/b/c/filename.ext"), "/a/b/c");
   test(getHead("A:\\a\\b\\c\\filename.ext"), "A:\\a\\b\\c");
   test(getHead("A:/a/b/c/filename.ext"), "A:/a/b/c");
   test(getHead("a/b"), "a");
   test(getHead("a"), "");
   test(getHead("/a"), "/");
   test(getHead("/"), "/");
}

static void testGetTail() {
   test(getTail("/a/b/c/filename.ext"), "filename.ext");
   test(getTail("A:\\a\\b\\c\\filename.ext"), "filename.ext");
   test(getTail("a/b"), "b");
   test(getTail("a"), "a");
   test(getTail("/a"), "a");
   test(getTail("/"), "");
}

static void testGetTempDirectory() {
   test(isDirectory(getTempDirectory()), true);
}
static void testDefaultExtension() {
   test(defaultExtension("filename", ".ext"), "filename.ext");
   test(defaultExtension("filename.old", ".new"), "filename.old");
   test(defaultExtension("filename.old", "*.new"), "filename.new");
   test(defaultExtension("a.b.c", "*.x"), "a.b.x");
}

static void testDeleteFile() {
    createDirectory("asdfqwer");
    test(isDirectory("asdfqwer"), true);
    deleteFile("asdfqwer");
    test(isDirectory("asdfqwer"), false);
    test(isFile("asdfqwer"), false);

    ofstream ofs;
    ofs.open("qwerasdf");
    ofs.close();
    test(isFile("qwerasdf"), true);
    deleteFile("qwerasdf");
    test(isFile("qwerasdf"), false);
}

static void testFileExists() {
   test(fileExists("EnglishWords.dat"), true);
   test(fileExists("xyzzy.cpp"), false);
}

static void testIsDirectory() {
   test(isDirectory("EnglishWords.dat"), false);
   test(isDirectory("."), true);
   test(isDirectory(".."), true);
   test(isDirectory("xyzzy.cpp"), false);
}

static void testIsFile() {
   test(isFile("EnglishWords.dat"), true);
   test(isFile("."), false);
   test(isFile("xyzzy.cpp"), false);
}

static void testIsSymbolicLink() {
#ifdef _WIN32
    if(CreateSymbolicLinkA("link_EnglishWords.dat", "EnglishWords.dat", 0) == 0) {
        cout << "CreateSymbolicLinkA failed: ";
        DWORD err = GetLastError();
        cout << err << endl;
        return;
    }

#else
    symlink("EnglishWords.dat", "link_EnglishWords.dat");
#endif
    test(isFile("link_EnglishWords.dat"), true);
    test(isSymbolicLink("link_EnglishWords.dat"), true);
    test(isSymbolicLink("EnglishWords.dat"), false);
    test(isSymbolicLink("."), false);
    test(isSymbolicLink("non/existent/file1234"), false);
}

static void testExpandPathname() {
   if (getDirectoryPathSeparator() == "/") {
      test(expandPathname("a/b"), "a/b");
      test(expandPathname("a\\b"), "a/b");
   } else {
      test(expandPathname("a/b"), "a\\b");
      test(expandPathname("a\\b"), "a\\b");
   }
}

static void testMatchFilenamePattern() {
   test(matchFilenamePattern("abc", "abc"), true);
   test(matchFilenamePattern("xxx", "abc"), false);
   test(matchFilenamePattern("ab", "abc"), false);
   test(matchFilenamePattern("abcd", "abc"), false);
   test(matchFilenamePattern("axc", "a?c"), true);
   test(matchFilenamePattern("ac", "a?c"), false);
   test(matchFilenamePattern("axxc", "a?c"), false);
   test(matchFilenamePattern("axxc", "a??c"), true);
   test(matchFilenamePattern("axc", "a*c"), true);
   test(matchFilenamePattern("ac", "a*c"), true);
   test(matchFilenamePattern("axxc", "a*c"), true);
   test(matchFilenamePattern("axxcd", "a*c"), false);
   test(matchFilenamePattern("axxbyyc", "a*b*c"), true);
   test(matchFilenamePattern("abc", "[abc]bc"), true);
   test(matchFilenamePattern("bbc", "[abc]bc"), true);
   test(matchFilenamePattern("xbc", "[abc]bc"), false);
   test(matchFilenamePattern("abc", "[^abc]bc"), false);
   test(matchFilenamePattern("bbc", "[^abc]bc"), false);
   test(matchFilenamePattern("xbc", "[^abc]bc"), true);
   test(matchFilenamePattern("abc", "[a-c]bc"), true);
   test(matchFilenamePattern("bbc", "[a-c]bc"), true);
   test(matchFilenamePattern("xbc", "[a-c]bc"), false);
   test(matchFilenamePattern("abc", "[^a-c]bc"), false);
   test(matchFilenamePattern("bbc", "[^a-c]bc"), false);
   test(matchFilenamePattern("xbc", "[^a-c]bc"), true);
}
