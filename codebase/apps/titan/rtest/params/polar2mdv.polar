########################################################################
# parameters file for polar2mdv
#
# Mike Dixon RAP NCAR Boulder Colorado USA
#
# June 1991
#
#########################################################################

#
# debugging
#

polar2mdv.debug: false
polar2mdv.malloc_debug_level: 0

#
# file generation. The program will write local and remote files
# independently. cartesianize is usually set to TRUE, if not a
# radial data file is produced.
# 

polar2mdv.run_length_encode: true
polar2mdv.rdata_dir: $(RTEST_HOME)/polar_data
polar2mdv.hostname: local
polar2mdv.local_tmp_dir: $(HOME)
polar2mdv.output_file_ext: mdv
polar2mdv.rc_table_path: $(RTEST_HOME)/tables/rc_table.polar

#
# clutter removal
#

polar2mdv.remove_clutter: false
polar2mdv.clutter_table_path: $(RTEST_HOME)/tables/clutter_table.polar

#
#  field positions for processed data
#

polar2mdv.nfields_processed: 5
polar2mdv.field_positions: 0 1 2 3 4

#
# signal-to-noise processing. The threshold is applied to the
# radial data, and any runs which are shorter than min_valid_run
# are deleted.
#

polar2mdv.check_sn: false
polar2mdv.sn_threshold: 10.0
polar2mdv.min_valid_run: 5

#
# if transmit_cart_data is set, will transmit data to cart slave
# for realtime display purposes. This program acts as a server
# for cart slave.
#

polar2mdv.transmit_cart_data: false
polar2mdv.max_packet_length: 20000
polar2mdv.check_clients_interval: 5.0
polar2mdv.port: 41100

#
# server mapper details for regsitering this server
# and process
#

polar2mdv.info: Cartesian data server
polar2mdv.instance: polar
polar2mdv.servmap_host1: local
polar2mdv.servmap_host2: none

#
# communication with polar_ingest via shared memory
#

polar2mdv.shmem_key: 41100
polar2mdv.shmem_read_wait: 0.1

#
# if auto_mid_time is set, the volume will be given a time
# stamp mid-way from start to end. Otherwise the time stamp
# will be age_at_end_of_volume before the end time
#

polar2mdv.auto_mid_time: true
polar2mdv.age_at_end_of_volume: 180

#
# if swap_times_if_incorrect is set, the data start and end
# times will be swapped it they are in the reverse order
#

polar2mdv.swap_times_if_incorrect: false

#
# The azimuth_offset is added to the azimuth angles prior to
# rounding to get the azimuth number
#

polar2mdv.azimuth_offset: 0.5

#
# max_missing_beams is the max number of missing beams allowed for
# a valid volume. If this is exceeded, the file will not be written.
#

polar2mdv.check_missing_beams: true
polar2mdv.report_missing_beams: true
polar2mdv.max_missing_beams: 500

#
# If use_repeated_elevations is true, repeated elevations
# will overwrite previous ones. If use_max_on_repeat is
# true, the max will be taken on repeat instead of overwriting.
#

polar2mdv.use_repeated_elevations: false
polar2mdv.use_max_on_repeat: true


