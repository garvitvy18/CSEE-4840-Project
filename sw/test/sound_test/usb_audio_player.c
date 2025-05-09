// usb_audio_player.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libusb-1.0/libusb.h>
#include <mpg123.h>

// callback invoked by libusb when an isochronous transfer completes
static void LIBUSB_CALL transfer_cb(struct libusb_transfer *xfr)
{
    free(xfr->buffer);
    libusb_free_transfer(xfr);
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr,
          "Usage: %s <vendor_id> <product_id> <file.mp3>\n"
          "Example: %s 0x077d 0x07af Tetris.mp3\n",
          argv[0], argv[0]);
        return 1;
    }

    uint16_t vid = (uint16_t)strtol(argv[1], NULL, 0);
    uint16_t pid = (uint16_t)strtol(argv[2], NULL, 0);
    const char *mp3_path = argv[3];

    // --- libusb init ---
    libusb_context *ctx = NULL;
    if (libusb_init(&ctx) < 0) {
        perror("libusb_init");
        return 1;
    }
    libusb_set_debug(ctx, 0);

    libusb_device_handle *dev = libusb_open_device_with_vid_pid(ctx, vid, pid);
    if (!dev) {
        fprintf(stderr, "Error: cannot open device %04x:%04x\n", vid, pid);
        libusb_exit(ctx);
        return 1;
    }

    // find the audio‐streaming interface + iso OUT endpoint
    struct libusb_config_descriptor *cfg = NULL;
    libusb_get_active_config_descriptor(libusb_get_device(dev), &cfg);

    int  audio_iface  = -1;
    int  audio_altset = -1;
    uint8_t ep_addr   = 0;
    uint16_t ep_maxpkt= 0;

    for (int i = 0; i < cfg->bNumInterfaces && audio_iface < 0; i++) {
        const struct libusb_interface *intf = &cfg->interface[i];
        for (int a = 0; a < intf->num_altsetting; a++) {
            const struct libusb_interface_descriptor *alt = &intf->altsetting[a];
            if (alt->bInterfaceClass == LIBUSB_CLASS_AUDIO &&
                alt->bInterfaceSubClass == 2 /* AUDIO_STREAMING */)
            {
                for (int e = 0; e < alt->bNumEndpoints; e++) {
                    const struct libusb_endpoint_descriptor *ed = &alt->endpoint[e];
                    if ((ed->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK)
                          == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS &&
                        (ed->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK)
                          == LIBUSB_ENDPOINT_OUT)
                    {
                        audio_iface   = alt->bInterfaceNumber;
                        audio_altset  = alt->bAlternateSetting;
                        ep_addr       = ed->bEndpointAddress;
                        ep_maxpkt     = ed->wMaxPacketSize;
                        break;
                    }
                }
            }
            if (audio_iface >= 0) break;
        }
    }
    libusb_free_config_descriptor(cfg);

    if (audio_iface < 0) {
        fprintf(stderr, "Error: no Audio‑Streaming interface found\n");
        libusb_close(dev);
        libusb_exit(ctx);
        return 1;
    }

    // detach kernel driver, claim & set alt‑setting
    libusb_detach_kernel_driver(dev, audio_iface);
    if (libusb_claim_interface(dev, audio_iface) < 0) {
        fprintf(stderr, "Error: cannot claim interface %d\n", audio_iface);
        libusb_close(dev);
        libusb_exit(ctx);
        return 1;
    }
    libusb_set_interface_alt_setting(dev, audio_iface, audio_altset);

    // --- mpg123 init ---
    mpg123_init();
    int err = 0;
    mpg123_handle *mh = mpg123_new(NULL, &err);
    if (!mh) {
        fprintf(stderr, "mpg123_new() failed: %s\n", mpg123_plain_strerror(err));
        goto cleanup_usb;
    }
    mpg123_open(mh, mp3_path);
    mpg123_format(mh, 44100, MPG123_STEREO, MPG123_ENC_SIGNED_16);

    // decode & submit loop
    size_t buf_size = ep_maxpkt * 32;
    unsigned char *decode_buf = malloc(buf_size);
    size_t bytes_decoded = 0;

    while (mpg123_read(mh, decode_buf, buf_size, &bytes_decoded) == MPG123_OK
           && bytes_decoded > 0)
    {
        int num_pkts = (bytes_decoded + ep_maxpkt - 1) / ep_maxpkt;
        struct libusb_transfer *xfr = libusb_alloc_transfer(num_pkts);
        unsigned char *xfer_buf = malloc(bytes_decoded);
        memcpy(xfer_buf, decode_buf, bytes_decoded);

        libusb_fill_iso_transfer(
          xfr,
          dev,
          ep_addr,
          xfer_buf,
          (int)bytes_decoded,
          num_pkts,
          transfer_cb,
          xfer_buf,
          1000
        );
        libusb_set_iso_packet_lengths(xfr, ep_maxpkt);

        if (libusb_submit_transfer(xfr) < 0) {
            fprintf(stderr, "Error: submit_transfer failed\n");
            free(xfer_buf);
            libusb_free_transfer(xfr);
            break;
        }
        libusb_handle_events(ctx);
    }

    // drain
    libusb_handle_events_timeout_completed(ctx, &(struct timeval){1,0}, NULL);

    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();
    free(decode_buf);

cleanup_usb:
    libusb_release_interface(dev, audio_iface);
    libusb_close(dev);
    libusb_exit(ctx);
    return 0;
}