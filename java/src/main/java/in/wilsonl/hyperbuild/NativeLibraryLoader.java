/*
 * Code in this file is shortened from https://github.com/adamheinrich/native-utils/blob/e6ea34b98e433bc67f2ad82a69f8f023d764e846/src/main/java/cz/adamh/utils/NativeUtils.java.
 *
 * Class NativeUtils is published under the The MIT License:
 *
 * Copyright (c) 2012 Adam Heinrich <adam@adamh.cz>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
package in.wilsonl.hyperbuild;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.FileSystemNotFoundException;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.ProviderNotFoundException;
import java.nio.file.StandardCopyOption;

import static java.lang.String.format;

public class NativeLibraryLoader {
  private NativeLibraryLoader() {
  }

  public static void loadLibraryFromJar(String filename) throws IOException {
    File temp = File.createTempFile("hyperbuild-java-nativelib", filename);

    try (InputStream is = NativeLibraryLoader.class.getResourceAsStream(format("/%s", filename))) {
      Files.copy(is, temp.toPath(), StandardCopyOption.REPLACE_EXISTING);
      System.load(temp.getAbsolutePath());
    } finally {
      if (isPosixCompliant()) {
        // Assume POSIX compliant file system, can be deleted after loading.
        temp.delete();
      } else {
        // Assume non-POSIX, and don't delete until last file descriptor closed.
        temp.deleteOnExit();
      }
    }
  }

  private static boolean isPosixCompliant() {
    try {
      return FileSystems.getDefault().supportedFileAttributeViews().contains("posix");
    } catch (FileSystemNotFoundException | ProviderNotFoundException | SecurityException e) {
      return false;
    }
  }
}
