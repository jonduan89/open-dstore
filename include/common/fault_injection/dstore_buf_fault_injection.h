/*
 * Copyright (C) 2026 Huawei Technologies Co.,Ltd.
 *
 * dstore is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * dstore is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. if not, see <https://www.gnu.org/licenses/>.
 *
 * ---------------------------------------------------------------------------------
 *
 * dstore_buf_fault_injection.h
 *
 * ---------------------------------------------------------------------------------------
 */
#ifndef DSTORE_BUF_FAULT_INJECTION_H
#define DSTORE_BUF_FAULT_INJECTION_H

#include "fault_injection/fault_injection.h"


namespace DSTORE {
enum class DstoreBufMgrFI {
    /* Buffer ring fault injection points */
    BUFRING_TRY_FLUSH_FAIL,
    BUFRING_MAKE_CR_FREE_FAIL,
    BUFRING_BASE_UNABLE_REUSE,
    BUFRING_REUSE_DIRTY_BUF,
    BUFRING_REUSE_BUF_IN_HASHTABLE,

    /* Buffer table fault injection points */
    BUFTABLE_INSERT_SYNC1,
    BUFTABLE_INSERT_SYNC2,
    BUFTABLE_INSERT_SYNC3,
    BUFTABLE_INSERT_SYNC4,
    BUFTABLE_INSERT_SYNC5,
    BUFTABLE_REMOVE_SYNC1,
    BUFTABLE_REMOVE_SYNC2,

    /* Local buffer manager fault injection points */
    READ_BLOCK_FAULT,
    WRITE_BLOCK_FAULT
};

}  // namespace DSTORE

#endif  // DSTORE_BUF_FAULT_INJECTION_H
