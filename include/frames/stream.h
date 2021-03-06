/*  Copyright 2014 Mandible Games
    
    This file is part of Frames.
    
    Please see the COPYING file for detailed licensing information.
    
    Frames is dual-licensed software. It is available under both a
    commercial license, and also under the terms of the GNU General
    Public License as published by the Free Software Foundation, either
    version 3 of the License, or (at your option) any later version.

    Frames is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Frames.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef FRAMES_STREAM
#define FRAMES_STREAM

#include "frames/noncopyable.h"
#include "frames/ptr.h"
#include "frames/types.h"

#include <string>
#include <cstdio>
#include <vector>

namespace Frames {
  class Stream;
  typedef Ptr<Stream> StreamPtr;

  class StreamFile;
  typedef Ptr<StreamFile> StreamFilePtr;

  class StreamBuffer;
  typedef Ptr<StreamBuffer> StreamBufferPtr;

  /// Interface for reading serialized data from a generic source.
  /** Frames Streams do not currently support any sort of asynchronous reading; this is planned for the future. */
  class Stream : public Refcountable<Stream> {
  public:
    Stream() { };
    virtual ~Stream() { };

    /// Reads a number of bytes.
    /** Returns the number of bytes successfully read. If the return value is not equal to the bytes parameter then it indicates either EOF or an error condition. */
    virtual int64_t Read(unsigned char *target, int64_t bytes) = 0;

    /// Seeks the read pointer to a given location in the file.
    /** Returns true if the read pointer was successfully moved. Failure leaves the read pointer in an undefined position. */
    virtual bool Seek(int64_t offset) = 0;

    /// Indicates whether Seek is valid on this stream.
    /** A "false" return value means that all calls to Seek will fail. */
    virtual bool Seekable() const = 0;
  };

  /// Implementation of Stream reading from standard disk files.
  /** See Stream for detailed function documentation. */
  class StreamFile : public Stream {
  public:
    /// Creates a StreamFile referring to a given file.
    /** Returns NULL if the file does not exist or cannot be read. */
    static StreamFilePtr Create(const std::string &fname);
    virtual ~StreamFile() FRAMES_OVERRIDE;

    virtual int64_t Read(unsigned char *target, int64_t bytes) FRAMES_OVERRIDE;
    virtual bool Seek(int64_t offset) FRAMES_OVERRIDE;
    virtual bool Seekable() const FRAMES_OVERRIDE;

  private:
    StreamFile(std::FILE *file);

    std::FILE *m_file;
  };
  
  /// Implementation of Stream reading from in-memory data.
  /** See Stream for detailed function documentation. */
  class StreamBuffer : public Stream {
  public:
    /// Creates a StreamBuffer referring to given data.
    /** For simplicity, this data is copied inside the StreamBuffer. */
    static StreamBufferPtr Create(const std::vector<unsigned char> &data);
    virtual ~StreamBuffer() FRAMES_OVERRIDE;

    virtual int64_t Read(unsigned char *target, int64_t bytes) FRAMES_OVERRIDE;
    virtual bool Seek(int64_t offset) FRAMES_OVERRIDE;
    virtual bool Seekable() const FRAMES_OVERRIDE;

  private:
    StreamBuffer(const std::vector<unsigned char> &data);

    std::vector<unsigned char> m_data;
    int64_t m_index;
  };
}

#endif

