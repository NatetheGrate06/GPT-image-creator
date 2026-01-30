# GPT-image-creator
A low-level systems project that programmatically generates a GUID Partition Table (GPT) disk image from scratch by writing raw, byte-accurate disk metadata. The project constructs a fully valid GPT layout, including the protective MBR, primary and backup GPT headers, and partition entry arrays, adhering strictly to the UEFI specification.
