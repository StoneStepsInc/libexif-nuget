#include <cstdlib>
#include <cstdio>
#include <string>
#include <libexif/exif-data.h>

int main(int argc, char **argv)
{
   if(argc < 2) {
      printf("Usage: sample-libexif image.jpg\n");
      return 1;
   }

   ExifData* ed = exif_data_new_from_file(argv[1]);

   if(!ed) {
      printf("Cannot read EXIF in file %s\n", argv[1]);
      return 2;
   }

   char buf[60];

   std::string make;    // camera make for the maker notes prefix

   ExifEntry *entry = exif_content_get_entry(ed->ifd[EXIF_IFD_0], EXIF_TAG_MAKE);

   exif_entry_get_value(entry, buf, sizeof(buf));

   make = buf;

   // print all generic EXIF tags
   for(size_t i = 0; i < EXIF_IFD_COUNT; i++) {
      if(ed->ifd[i]) {
         for(size_t k = 0; k < ed->ifd[i]->count; k++) {
            entry = ed->ifd[i]->entries[k];

            if(entry) {
               exif_entry_get_value(entry, buf, sizeof(buf));
               printf("%s: %s\t: %s\n", exif_ifd_get_name((ExifIfd) i), exif_tag_get_name_in_ifd(entry->tag, (ExifIfd) i), buf);
            }
         }
      }
   }

   // check if there are any maker notes
   ExifMnoteData *mn = exif_data_get_mnote_data(ed);

   if(mn) {                                         
      size_t mn_count = exif_mnote_data_count(mn);

      for(unsigned int i = 0; i < mn_count; i++) {
         // some names are returned as NULL
         const char *name = exif_mnote_data_get_name(mn, i);

         if(name && exif_mnote_data_get_value(mn, i, buf, sizeof(buf)))
            printf("%s: %s\t: %s\n", make.c_str(), exif_mnote_data_get_name(mn, i), buf);
      }
   }

   exif_data_unref(ed);

   return EXIT_SUCCESS;
}
