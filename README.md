# LogRatioChangeDetection
An algorithm created to compute changes from the log ratio using GDAL and OpenMP
The program curently accepts the input created from the RangeDoppler Terrain Correction after image calibration.

Usage: LogRatioChangeDetection image1 image2 polarization output_file threshold (optional)

polarization values: VV or VH

output file should be in .tif format
