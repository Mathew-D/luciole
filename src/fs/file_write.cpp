#include "fs/file_write.h"

#include "fs/gio_interop.h"
#include "fs/gio_ptr.h"

#include <gio/gio.h>
#include <string>

void luciole::createFileWithBytes(
    const Location& destination, std::string_view bytes, std::move_only_function<void(JobResult<Location>)> done
) {
  runJob<Location>(
      [destination, bytes = std::string(bytes)] -> Location {
        const auto fail = [](GError* raw) -> FileErrorException {
          const GErrorPtr error(raw);
          return FileErrorException(error ? fileError(*error) : FileError{FileErrorKind::Other, "Write failed."});
        };
        const GioPtr<GFile> file(gioFile(destination));
        GError* error = nullptr;
        const GioPtr<GFileOutputStream> output(g_file_create(file.get(), G_FILE_CREATE_NONE, nullptr, &error));
        if (!output) {
          throw fail(error);
        }
        const auto discard = [&] {
          g_file_delete(file.get(), nullptr, nullptr);
          return fail(error);
        };
        if (!g_output_stream_write_all(
                G_OUTPUT_STREAM(output.get()), bytes.data(), bytes.size(), nullptr, nullptr, &error
            )) {
          throw discard();
        }
        if (!g_output_stream_close(G_OUTPUT_STREAM(output.get()), nullptr, &error)) {
          throw discard();
        }
        return destination;
      },
      std::move(done)
  );
}
