########################################################################
# parameters file for polar_ingest
#
# Mike Dixon RAP NCAR Boulder Colorado USA
#
# December 1991
#
#########################################################################

#
# debugging
#

polar_ingest.debug: false
polar_ingest.malloc_debug_level: 0

#
# process instance
#

polar_ingest.instance: rtest

#
# these are the beam intervals at which the header and summary are
# printed out
#

polar_ingest.header_interval: 1
polar_ingest.summary_interval: 360

#
# time correction
#
# if time_override is true, current unix time is used to overwrite data time.
# if time_correction (secs) is non_zero, correction is added to data time.
#

polar_ingest.time_override: false
polar_ingest.time_correction: 0

#
# polar_ingest starts up polar2mdv using this
# command
#

polar_ingest.start_polar2mdv: true
polar_ingest.polar2mdv_command_line: polar2mdv -params polar2mdv.polar &

#
# end_of_vol_decision may be set to end_of_given_tilt or
# end_of_vol_flag. In the former case the end of volume will
# occur at the end of last_tilt_in_vol. In the latter case the
# end of volume will be triggered by the end_of_vol_flag in
# the data stream
#

polar_ingest.end_of_vol_decision: end_of_vol_flag
polar_ingest.last_tilt_in_vol: 6

#
# shared memory communication with polar2mdv
#

polar_ingest.shmem_key: 41100
polar_ingest.nbeams_buffer: 100

#
# devices for reading data
#
# device can be tape, tcpip or udp
#

polar_ingest.device: tcpip

#
# tape device params
#

polar_ingest.tape_read_wait: 0.0
polar_ingest.tape_name: /dev/rst2

#
# tcpip device params
#

polar_ingest.tcpip_host: local
polar_ingest.tcpip_port: 49000

#
# udp device params
#

polar_ingest.udp_port: 54321

#
# header type: lincoln, rp7, gate_data
#

polar_ingest.header_type: gate_data

#
# modes - will filter out beams which do not match the
# selected modes
#

polar_ingest.surveillance_mode: true
polar_ingest.rhi_mode: false
polar_ingest.sector_mode: false

#
# optional checks
#

polar_ingest.check_gate_spacing: false
polar_ingest.target_gate_spacing: 0.225

polar_ingest.check_tilt_number: false
polar_ingest.tilt_numbers: 1 2 3 4 5

polar_ingest.check_elev_limits: false
polar_ingest.min_elevation: 0.0
polar_ingest.max_elevation: 90.0

#
# set receiver noise in units of dBZ at a range
# of 100km
#

polar_ingest.noise_dbz_at_100km: -8.0

#
# radar parameters
# 

polar_ingest.note: Mile High radar data, RAP, NCAR, Boulder, CO, USA
polar_ingest.radar_id: 5
polar_ingest.radar_name: Mile_High(Denver)
polar_ingest.ns_hemisphere: north
polar_ingest.ew_hemisphere: west
polar_ingest.samples_per_beam: 45

#
# ngates_out is number of gates to be placed in shared memory buffer.
# ngates_dropped is number of gates to be dropped from start of
# beam before placing in shared memory.
# Note: ngates_out + ngates_dropped <= ngates_in
#

polar_ingest.ngates_out: 660
polar_ingest.ngates_dropped: 0

#
# nfields_in is number of fields coming in - all incoming fields are
# placed in the shared memory buffer
#

polar_ingest.nfields_in: 5
polar_ingest.field_names: DBZ,VEL,EL,AZ,RANGE
polar_ingest.field_units: dBZ,m/s,deg,deg,km
polar_ingest.field_transform: dbz,none,none,none,none

#
# field position of dbz field
#

polar_ingest.dbz_field_pos: 0

#
# optinally a flag field exists in the data stream. If so, this
# field may be used to remove unwanted data.
#

polar_ingest.apply_flags: false
polar_ingest.flag_field_pos: 4

#
# If use_bit_mask is set, then flag_check_val is a decimal
# integer in which are set those bits which correspond to
# undesired flag vals.
#
# If use_bit_mask is not set, then if the undesired flag value
# lie between flag_value_min and flag_value_max
#

polar_ingest.use_bit_mask: false
polar_ingest.flag_check_val: 0

polar_ingest.flag_value_min: 0
polar_ingest.flag_value_max: 0

#
# Option to write RDI message queue.
# The shmem key also controls the size of the
# shared mem buffer used. The last 4 digits of the
# key gives the buffer size in Kbytes,
# i.e. shmem_size = ((key % 10000) * 1024) bytes
#

polar_ingest.write_rdi_mmq: false
polar_ingest.rdi_mmq_key: 3740201
