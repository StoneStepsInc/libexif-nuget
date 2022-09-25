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

   ExifData *ed = exif_data_new_from_file(argv[1]);

   if(!ed) {
      printf("Cannot read EXIF in file %s\n", argv[1]);
      return 2;
   }

   char buf[60];              // long entries will be truncated

   std::string tag_make;      // camera make for the maker notes prefix

   ExifEntry *entry = exif_content_get_entry(ed->ifd[EXIF_IFD_0], EXIF_TAG_MAKE);
   exif_entry_get_value(entry, buf, sizeof(buf));
   tag_make = buf;

   // byte order may be used to interpret numeric entry values
   ExifByteOrder byte_order = exif_data_get_byte_order(ed);
   entry = exif_content_get_entry(ed->ifd[EXIF_IFD_EXIF], EXIF_TAG_FNUMBER);

   if(entry && entry->format == EXIF_FORMAT_RATIONAL) {
      ExifRational rt_value = exif_get_rational(entry->data, byte_order);
      printf("EXIF_TAG_FNUMBER: %.1lf\n", (double) rt_value.numerator/ (double) rt_value.denominator);
   }

   // print all generic EXIF tags
   for(size_t i = 0; i < EXIF_IFD_COUNT; i++) {
      if(ed->ifd[i]) {
         for(size_t k = 0; k < ed->ifd[i]->count; k++) {
            entry = ed->ifd[i]->entries[k];

            if(entry) {
               exif_entry_get_value(entry, buf, sizeof(buf));
               printf("%s: %s (%s/%u/%u*%lu)\t: %s\n", exif_ifd_get_name((ExifIfd) i), exif_tag_get_name_in_ifd(entry->tag, (ExifIfd) i),
                                                         exif_format_get_name(entry->format), entry->size,
                                                         exif_format_get_size(entry->format), entry->components,
                                                         buf);
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
            printf("%s: %s (%s/%u/%u*%lu)\t: %s\n", tag_make.c_str(), exif_mnote_data_get_name(mn, i),
                                                      exif_format_get_name(entry->format), entry->size,
                                                      exif_format_get_size(entry->format), entry->components,
                                                      buf);
      }
   }

   exif_data_unref(ed);

   return EXIT_SUCCESS;
}
