/* 
 *
 * Copyright  2012, 2013  Freescale Semiconductor
 *
 *
 * Licensed under the Apache License, version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
*/

/*
 *
 * File name: cmfile.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: This file contains wrapper functions that in turn
 *              calls underlying file-system specific API's.
 * 
 */

#ifndef _UCMFILE_H
#define _UCMFILE_H


#define UCMFILE_MODE_READ            (O_RDONLY)
#define UCMFILE_MODE_APPEND          (O_APPEND)
#define UCMFILE_MODE_CREAT           (O_CREAT)
#define UCMFILE_MODE_TRUNC           (O_TRUNC)
#define UCMFILE_MODE_WRITE           (O_WRONLY | O_CREAT)
#define UCMFILE_MODE_READ_CREATE     (UCMFILE_MODE_READ | UCMFILE_MODE_CREAT)
#define UCMFILE_MODE_READ_WRITE      (UCMFILE_MODE_READ | UCMFILE_MODE_WRITE)
#define UCMFILE_MODE_WRITE_APPEND    (UCMFILE_MODE_APPEND | UCMFILE_MODE_WRITE)
#define UCMFILE_MODE_RDWR_TRUNCATE   (UCMFILE_MODE_READ_WRITE | UCMFILE_MODE_TRUNC)

#define UCMFILE_SEEK_BEG     (SEEK_SET)
#define UCMFILE_SEEK_CUR     (SEEK_CUR)
#define UCMFILE_SEEK_END     (SEEK_END)

#ifndef S_IRWXU
#define S_IRWXU                  0x0700
#endif
 
int32_t cm_file_open (int32_t fs_id, unsigned char * file_name_p, int32_t mode);
int32_t cm_file_close (int32_t fs_id, int32_t fd);
int32_t cm_file_create (int32_t fs_id, unsigned char * file_name_p);
int32_t cm_file_read (int32_t fs_id, int32_t fd, unsigned char * buf_p,
                     uint32_t count_ui);
int32_t cm_file_write (int32_t fs_id, int32_t fd, unsigned char * buf_p,
                      uint32_t count_ui);
int32_t cm_file_seek (int32_t fs_id, int32_t fd, int32_t position,
                     int32_t offset);
int32_t cm_file_remove (int32_t fs_id, unsigned char * file_name_p);
int32_t cm_file_flush (int32_t fs_id, int32_t fd);
int32_t cm_file_rename (unsigned char * old_file_name_p, unsigned char * new_file_name_p);
int32_t cm_file_is_file_system_exist (uint32_t start_addr_ui, uint32_t length_ui);
int32_t cm_file_tell (int32_t fs_id, int32_t fd);
int32_t cm_file_verify_checksum (int32_t fs_id, unsigned char * file_name_p);
int32_t cm_file_format_file_system (int32_t fs_id, uint32_t max_files_ui,
                                 int32_t mode);
int32_t cm_file_is_format_needed (int32_t fs_id);

#endif /* CM_SUPPORT */
