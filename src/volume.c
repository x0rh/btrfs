/* Copyright (c) Mark Harmstone 2016
 * 
 * This file is part of WinBtrfs.
 * 
 * WinBtrfs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public Licence as published by
 * the Free Software Foundation, either version 3 of the Licence, or
 * (at your option) any later version.
 * 
 * WinBtrfs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public Licence for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public Licence
 * along with WinBtrfs.  If not, see <http://www.gnu.org/licenses/>. */

#include "btrfs_drv.h"
#include <mountdev.h>
#include <ntddvol.h>
#include <ntddstor.h>
#include <ntdddisk.h>

#define IOCTL_VOLUME_IS_DYNAMIC     CTL_CODE(IOCTL_VOLUME_BASE, 18, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VOLUME_POST_ONLINE    CTL_CODE(IOCTL_VOLUME_BASE, 25, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

extern PDRIVER_OBJECT drvobj;
extern ERESOURCE volume_list_lock;
extern LIST_ENTRY volume_list;

NTSTATUS STDCALL vol_create(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    TRACE("(%p, %p)\n", DeviceObject, Irp);

    Irp->IoStatus.Information = FILE_OPENED;
    
    return STATUS_SUCCESS;
}

NTSTATUS STDCALL vol_close(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    TRACE("(%p, %p)\n", DeviceObject, Irp);
    
    Irp->IoStatus.Information = 0;

    return STATUS_SUCCESS;
}

NTSTATUS STDCALL vol_read(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    ERR("(%p, %p)\n", DeviceObject, Irp);

    // FIXME

    return STATUS_INVALID_DEVICE_REQUEST;
}

NTSTATUS STDCALL vol_write(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    ERR("(%p, %p)\n", DeviceObject, Irp);

    // FIXME

    return STATUS_INVALID_DEVICE_REQUEST;
}

NTSTATUS STDCALL vol_query_information(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    ERR("(%p, %p)\n", DeviceObject, Irp);

    // FIXME

    return STATUS_INVALID_DEVICE_REQUEST;
}

NTSTATUS STDCALL vol_set_information(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    ERR("(%p, %p)\n", DeviceObject, Irp);

    // FIXME

    return STATUS_INVALID_DEVICE_REQUEST;
}

NTSTATUS STDCALL vol_query_ea(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    ERR("(%p, %p)\n", DeviceObject, Irp);

    // FIXME

    return STATUS_INVALID_DEVICE_REQUEST;
}

NTSTATUS STDCALL vol_set_ea(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    ERR("(%p, %p)\n", DeviceObject, Irp);

    // FIXME

    return STATUS_INVALID_DEVICE_REQUEST;
}

NTSTATUS STDCALL vol_flush_buffers(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    ERR("(%p, %p)\n", DeviceObject, Irp);

    // FIXME

    return STATUS_INVALID_DEVICE_REQUEST;
}

NTSTATUS STDCALL vol_query_volume_information(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    ERR("(%p, %p)\n", DeviceObject, Irp);

    // FIXME

    return STATUS_INVALID_DEVICE_REQUEST;
}

NTSTATUS STDCALL vol_set_volume_information(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    ERR("(%p, %p)\n", DeviceObject, Irp);

    // FIXME

    return STATUS_INVALID_DEVICE_REQUEST;
}

NTSTATUS STDCALL vol_cleanup(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    TRACE("(%p, %p)\n", DeviceObject, Irp);

    Irp->IoStatus.Information = 0;

    return STATUS_SUCCESS;
}

NTSTATUS STDCALL vol_directory_control(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    ERR("(%p, %p)\n", DeviceObject, Irp);

    // FIXME

    return STATUS_INVALID_DEVICE_REQUEST;
}

static NTSTATUS vol_fsctl_request(PDEVICE_OBJECT DeviceObject, PIRP Irp, ULONG code, BOOL user) {
    switch (code) {
        case FSCTL_QUERY_DEPENDENT_VOLUME:
            ERR("unhandled control code FSCTL_QUERY_DEPENDENT_VOLUME\n");
        break;
        
        default:
            ERR("unhandled control code %x\n", code);
        break;
    }
    
    return STATUS_INVALID_DEVICE_REQUEST;
}

NTSTATUS STDCALL vol_file_system_control(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    PIO_STACK_LOCATION IrpSp = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS Status;
    
    ERR("(%p, %p)\n", DeviceObject, Irp);

    switch (IrpSp->MinorFunction) {
        case IRP_MN_KERNEL_CALL:
            TRACE("IRP_MN_KERNEL_CALL\n");
            
            Status = vol_fsctl_request(DeviceObject, Irp, IrpSp->Parameters.FileSystemControl.FsControlCode, FALSE);
            break;
            
        case IRP_MN_USER_FS_REQUEST:
            TRACE("IRP_MN_USER_FS_REQUEST\n");
            
            Status = vol_fsctl_request(DeviceObject, Irp, IrpSp->Parameters.FileSystemControl.FsControlCode, TRUE);
            break;
            
        default:
            Status = STATUS_INVALID_DEVICE_REQUEST;
            break;
    }

    return Status;
}

NTSTATUS STDCALL vol_lock_control(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    ERR("(%p, %p)\n", DeviceObject, Irp);

    // FIXME

    return STATUS_INVALID_DEVICE_REQUEST;
}

static NTSTATUS vol_query_device_name(volume_device_extension* vde, PIRP Irp) {
    PIO_STACK_LOCATION IrpSp = IoGetCurrentIrpStackLocation(Irp);
    PMOUNTDEV_NAME name;

    if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength < sizeof(MOUNTDEV_NAME)) {
        Irp->IoStatus.Information = sizeof(MOUNTDEV_NAME);
        return STATUS_BUFFER_TOO_SMALL;
    }

    name = Irp->AssociatedIrp.SystemBuffer;
    name->NameLength = vde->name.Length;

    if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength < offsetof(MOUNTDEV_NAME, Name[0]) + name->NameLength) {
        Irp->IoStatus.Information = sizeof(MOUNTDEV_NAME);
        return STATUS_BUFFER_OVERFLOW;
    }
    
    RtlCopyMemory(name->Name, vde->name.Buffer, vde->name.Length);

    Irp->IoStatus.Information = offsetof(MOUNTDEV_NAME, Name[0]) + name->NameLength;
    
    return STATUS_SUCCESS;
}

static NTSTATUS vol_query_unique_id(volume_device_extension* vde, PIRP Irp) {
    PIO_STACK_LOCATION IrpSp = IoGetCurrentIrpStackLocation(Irp);
    MOUNTDEV_UNIQUE_ID* mduid;

    if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength < sizeof(MOUNTDEV_UNIQUE_ID)) {
        Irp->IoStatus.Information = sizeof(MOUNTDEV_UNIQUE_ID);
        return STATUS_BUFFER_TOO_SMALL;
    }

    mduid = Irp->AssociatedIrp.SystemBuffer;
    mduid->UniqueIdLength = sizeof(BTRFS_UUID);

    if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength < offsetof(MOUNTDEV_UNIQUE_ID, UniqueId[0]) + mduid->UniqueIdLength) {
        Irp->IoStatus.Information = sizeof(MOUNTDEV_UNIQUE_ID);
        return STATUS_BUFFER_OVERFLOW;
    }

    RtlCopyMemory(mduid->UniqueId, &vde->uuid, sizeof(BTRFS_UUID));

    Irp->IoStatus.Information = offsetof(MOUNTDEV_UNIQUE_ID, UniqueId[0]) + mduid->UniqueIdLength;
    
    return STATUS_SUCCESS;
}

static NTSTATUS vol_is_dynamic(volume_device_extension* vde, PIRP Irp) {
    PIO_STACK_LOCATION IrpSp = IoGetCurrentIrpStackLocation(Irp);
    UINT8* buf;
    
    if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength == 0 || !Irp->AssociatedIrp.SystemBuffer)
        return STATUS_INVALID_PARAMETER;
    
    buf = (UINT8*)Irp->AssociatedIrp.SystemBuffer;
    
    *buf = 1;
    
    Irp->IoStatus.Information = 1;
    
    return STATUS_SUCCESS;
}

static NTSTATUS vol_check_verify(volume_device_extension* vde) {
    NTSTATUS Status;
    LIST_ENTRY* le;
    
    ExAcquireResourceSharedLite(&vde->child_lock, TRUE);
    
    le = vde->children.Flink;
    while (le != &vde->children) {
        volume_child* vc = CONTAINING_RECORD(le, volume_child, list_entry);
        
        Status = dev_ioctl(vc->devobj, IOCTL_STORAGE_CHECK_VERIFY, NULL, 0, NULL, 0, FALSE, NULL);
        if (!NT_SUCCESS(Status))
            goto end;
        
        le = le->Flink;
    }
    
    Status = STATUS_SUCCESS;
    
end:
    ExReleaseResourceLite(&vde->child_lock);
    
    return Status;
}

static NTSTATUS vol_get_disk_extents(volume_device_extension* vde, PIRP Irp) {
    PIO_STACK_LOCATION IrpSp = IoGetCurrentIrpStackLocation(Irp);
    LIST_ENTRY* le;
    ULONG num_extents = 0, i, max_extents = 1;
    NTSTATUS Status;
    VOLUME_DISK_EXTENTS *ext, *ext3;
    
    if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength < sizeof(VOLUME_DISK_EXTENTS))
        return STATUS_BUFFER_TOO_SMALL;
    
    ExAcquireResourceSharedLite(&vde->child_lock, TRUE);
    
    le = vde->children.Flink;
    while (le != &vde->children) {
        volume_child* vc = CONTAINING_RECORD(le, volume_child, list_entry);
        
        if (vc->offset == 0) { // passthrough
            VOLUME_DISK_EXTENTS ext2;
            
            Status = dev_ioctl(vc->devobj, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, &ext2, sizeof(VOLUME_DISK_EXTENTS), FALSE, NULL);
            if (!NT_SUCCESS(Status) && Status != STATUS_BUFFER_OVERFLOW) {
                ERR("IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS returned %08x\n", Status);
                goto end;
            }
            
            num_extents += ext2.NumberOfDiskExtents;
            
            if (ext2.NumberOfDiskExtents > max_extents)
                max_extents = ext2.NumberOfDiskExtents;
        } else
            num_extents++;
        
        le = le->Flink;
    }
    
    ext = Irp->AssociatedIrp.SystemBuffer;
    
    if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength < offsetof(VOLUME_DISK_EXTENTS, Extents[0]) + (num_extents * sizeof(DISK_EXTENT))) {
        Irp->IoStatus.Information = offsetof(VOLUME_DISK_EXTENTS, Extents[0]);
        ext->NumberOfDiskExtents = num_extents;
        Status = STATUS_BUFFER_OVERFLOW;
        goto end;
    }
    
    ext3 = ExAllocatePoolWithTag(PagedPool, offsetof(VOLUME_DISK_EXTENTS, Extents[0]) + (max_extents * sizeof(DISK_EXTENT)), ALLOC_TAG);
    if (!ext3) {
        ERR("out of memory\n");
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto end;
    }
    
    i = 0;
    ext->NumberOfDiskExtents = 0;
    
    le = vde->children.Flink;
    while (le != &vde->children) {
        volume_child* vc = CONTAINING_RECORD(le, volume_child, list_entry);
        
        if (vc->offset == 0) { // passthrough
            Status = dev_ioctl(vc->devobj, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, ext3,
                               offsetof(VOLUME_DISK_EXTENTS, Extents[0]) + (max_extents * sizeof(DISK_EXTENT)), FALSE, NULL);
            if (!NT_SUCCESS(Status)) {
                ERR("IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS returned %08x\n", Status);
                ExFreePool(ext3);
                goto end;
            }
            
            if (i + ext3->NumberOfDiskExtents > num_extents) {
                Irp->IoStatus.Information = offsetof(VOLUME_DISK_EXTENTS, Extents[0]);
                ext->NumberOfDiskExtents = i + ext3->NumberOfDiskExtents;
                Status = STATUS_BUFFER_OVERFLOW;
                goto end;
            }
            
            RtlCopyMemory(&ext->Extents[i], ext3->Extents, sizeof(DISK_EXTENT) * ext3->NumberOfDiskExtents);
            i += ext3->NumberOfDiskExtents;
        } else {
            ext->Extents[i].DiskNumber = vc->disk_num;
            ext->Extents[i].StartingOffset.QuadPart = vc->offset;
            ext->Extents[i].ExtentLength.QuadPart = vc->size;
            i++;
        }
        
        le = le->Flink;
    }
    
    ExFreePool(ext3);
    
    Status = STATUS_SUCCESS;
    
    ext->NumberOfDiskExtents = i;
    Irp->IoStatus.Information = offsetof(VOLUME_DISK_EXTENTS, Extents[0]) + (i * sizeof(DISK_EXTENT));
    
end:
    ExReleaseResourceLite(&vde->child_lock);
    
    return Status;
}

static NTSTATUS vol_is_writable(volume_device_extension* vde) {
    NTSTATUS Status;
    LIST_ENTRY* le;
    BOOL writable = FALSE;
    
    ExAcquireResourceSharedLite(&vde->child_lock, TRUE);
    
    le = vde->children.Flink;
    while (le != &vde->children) {
        volume_child* vc = CONTAINING_RECORD(le, volume_child, list_entry);
        
        Status = dev_ioctl(vc->devobj, IOCTL_DISK_IS_WRITABLE, NULL, 0, NULL, 0, TRUE, NULL);
        
        if (NT_SUCCESS(Status)) {
            writable = TRUE;
            break;
        } else if (Status != STATUS_MEDIA_WRITE_PROTECTED)
            goto end;
        
        le = le->Flink;
    }
    
    Status = writable ? STATUS_SUCCESS : STATUS_MEDIA_WRITE_PROTECTED;
    
end:
    ExReleaseResourceLite(&vde->child_lock);
    
    return STATUS_SUCCESS;
}

static NTSTATUS vol_get_length(volume_device_extension* vde, PIRP Irp) {
    PIO_STACK_LOCATION IrpSp = IoGetCurrentIrpStackLocation(Irp);
    GET_LENGTH_INFORMATION* gli;
    LIST_ENTRY* le;
    
    if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength < sizeof(GET_LENGTH_INFORMATION))
        return STATUS_BUFFER_TOO_SMALL;
    
    gli = (GET_LENGTH_INFORMATION*)Irp->AssociatedIrp.SystemBuffer;
    
    gli->Length.QuadPart = 0;
    
    ExAcquireResourceSharedLite(&vde->child_lock, TRUE);
    
    le = vde->children.Flink;
    while (le != &vde->children) {
        volume_child* vc = CONTAINING_RECORD(le, volume_child, list_entry);
        
        gli->Length.QuadPart += vc->size;
        
        le = le->Flink;
    }
    
    ExReleaseResourceLite(&vde->child_lock);
    
    Irp->IoStatus.Information = sizeof(GET_LENGTH_INFORMATION);
    
    return STATUS_SUCCESS;
}

NTSTATUS STDCALL vol_device_control(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    volume_device_extension* vde = DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION IrpSp = IoGetCurrentIrpStackLocation(Irp);
    
    TRACE("(%p, %p)\n", DeviceObject, Irp);

    Irp->IoStatus.Information = 0;
    
    switch (IrpSp->Parameters.DeviceIoControl.IoControlCode) {
        case IOCTL_MOUNTDEV_QUERY_DEVICE_NAME:
            return vol_query_device_name(vde, Irp);
            
        case IOCTL_MOUNTDEV_QUERY_UNIQUE_ID:
            return vol_query_unique_id(vde, Irp);
            
        case IOCTL_STORAGE_GET_DEVICE_NUMBER:
            TRACE("unhandled control code IOCTL_STORAGE_GET_DEVICE_NUMBER\n");
            break;

        case IOCTL_MOUNTDEV_QUERY_SUGGESTED_LINK_NAME:
            TRACE("unhandled control code IOCTL_MOUNTDEV_QUERY_SUGGESTED_LINK_NAME\n");
            break;

        case IOCTL_MOUNTDEV_QUERY_STABLE_GUID:
            TRACE("unhandled control code IOCTL_MOUNTDEV_QUERY_STABLE_GUID\n");
            break;

        case IOCTL_MOUNTDEV_LINK_CREATED:
            TRACE("unhandled control code IOCTL_MOUNTDEV_LINK_CREATED\n");
            break;

        case IOCTL_VOLUME_GET_GPT_ATTRIBUTES:
            ERR("unhandled control code IOCTL_VOLUME_GET_GPT_ATTRIBUTES\n");
            break;

        case IOCTL_VOLUME_IS_DYNAMIC:
            return vol_is_dynamic(vde, Irp);

        case IOCTL_VOLUME_ONLINE:
            TRACE("unhandled control code IOCTL_VOLUME_ONLINE\n");
            break;

        case IOCTL_VOLUME_POST_ONLINE:
            TRACE("unhandled control code IOCTL_VOLUME_POST_ONLINE\n");
            break;

        case IOCTL_DISK_GET_DRIVE_GEOMETRY:
            ERR("unhandled control code IOCTL_DISK_GET_DRIVE_GEOMETRY\n");
            break;

        case IOCTL_DISK_IS_WRITABLE:
            return vol_is_writable(vde);

        case IOCTL_DISK_GET_LENGTH_INFO:
            return vol_get_length(vde, Irp);
            
        case IOCTL_STORAGE_CHECK_VERIFY:
            return vol_check_verify(vde);

        case IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS:
            return vol_get_disk_extents(vde, Irp);

        default:
            ERR("unhandled control code %x\n", IrpSp->Parameters.DeviceIoControl.IoControlCode);
            break;
    }

    return STATUS_INVALID_DEVICE_REQUEST;
}

NTSTATUS STDCALL vol_shutdown(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    ERR("(%p, %p)\n", DeviceObject, Irp);

    // FIXME

    return STATUS_INVALID_DEVICE_REQUEST;
}

NTSTATUS STDCALL vol_query_security(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    ERR("(%p, %p)\n", DeviceObject, Irp);

    // FIXME

    return STATUS_INVALID_DEVICE_REQUEST;
}

NTSTATUS STDCALL vol_set_security(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    ERR("(%p, %p)\n", DeviceObject, Irp);

    // FIXME

    return STATUS_INVALID_DEVICE_REQUEST;
}

NTSTATUS STDCALL vol_power(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    PIO_STACK_LOCATION IrpSp = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS Status;
    
    ERR("(%p, %p)\n", DeviceObject, Irp);

    if (IrpSp->MinorFunction == IRP_MN_SET_POWER || IrpSp->MinorFunction == IRP_MN_QUERY_POWER)
        Irp->IoStatus.Status = STATUS_SUCCESS;
    
    Status = Irp->IoStatus.Status;
    PoStartNextPowerIrp(Irp);

    return Status;
}

NTSTATUS mountmgr_add_drive_letter(PDEVICE_OBJECT mountmgr, PUNICODE_STRING devpath) {
    NTSTATUS Status;
    ULONG mmdltsize;
    MOUNTMGR_DRIVE_LETTER_TARGET* mmdlt;
    MOUNTMGR_DRIVE_LETTER_INFORMATION mmdli;
    
    mmdltsize = offsetof(MOUNTMGR_DRIVE_LETTER_TARGET, DeviceName[0]) + devpath->Length;
    
    mmdlt = ExAllocatePoolWithTag(NonPagedPool, mmdltsize, ALLOC_TAG);
    if (!mmdlt) {
        ERR("out of memory\n");
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    
    mmdlt->DeviceNameLength = devpath->Length;
    RtlCopyMemory(&mmdlt->DeviceName, devpath->Buffer, devpath->Length);
    TRACE("mmdlt = %.*S\n", mmdlt->DeviceNameLength / sizeof(WCHAR), mmdlt->DeviceName);
    
    Status = dev_ioctl(mountmgr, IOCTL_MOUNTMGR_NEXT_DRIVE_LETTER, mmdlt, mmdltsize, &mmdli, sizeof(MOUNTMGR_DRIVE_LETTER_INFORMATION), FALSE, NULL);
    
    if (!NT_SUCCESS(Status))
        ERR("IOCTL_MOUNTMGR_NEXT_DRIVE_LETTER returned %08x\n", Status);
    else
        TRACE("DriveLetterWasAssigned = %u, CurrentDriveLetter = %c\n", mmdli.DriveLetterWasAssigned, mmdli.CurrentDriveLetter);
    
    return Status;
}

static __inline WCHAR hex_digit(UINT8 n) {
    if (n >= 0 && n <= 9)
        return n + '0';
    else
        return n - 0xa + 'a';
}

void add_volume_device(superblock* sb, PDEVICE_OBJECT mountmgr, PUNICODE_STRING devpath, UINT64 offset, UINT64 length, ULONG disk_num,
                       ULONG part_num, PUNICODE_STRING partname) {
    NTSTATUS Status;
    LIST_ENTRY* le;
    UNICODE_STRING volname;
    PDEVICE_OBJECT voldev, DeviceObject;
    volume_device_extension* vde = NULL;
    int i, j;
    BOOL new_vde = FALSE;
    volume_child* vc;
    PFILE_OBJECT FileObject;
    
    if (devpath->Length == 0)
        return;
    
    ExAcquireResourceExclusiveLite(&volume_list_lock, TRUE);
    
    le = volume_list.Flink;
    while (le != &volume_list) {
        volume_device_extension* vde2 = CONTAINING_RECORD(le, volume_device_extension, list_entry);
        
        if (RtlCompareMemory(&vde2->uuid, &sb->uuid, sizeof(BTRFS_UUID)) == sizeof(BTRFS_UUID)) {
            vde = vde2;
            break;
        }
        
        le = le->Flink;
    }
    
    Status = IoGetDeviceObjectPointer(devpath, FILE_READ_ATTRIBUTES, &FileObject, &DeviceObject);
    if (!NT_SUCCESS(Status)) {
        ERR("IoGetDeviceObjectPointer returned %08x\n", Status);
        ExReleaseResourceLite(&volume_list_lock);
        goto end;
    }
    
    if (!vde) {
        PDEVICE_OBJECT pdo = NULL;
        BOOL registered = FALSE;
        
        Status = IoReportDetectedDevice(drvobj, InterfaceTypeUndefined, 0xFFFFFFFF, 0xFFFFFFFF,
                                        NULL, NULL, 0, &pdo);
        if (!NT_SUCCESS(Status)) {
            ERR("IoReportDetectedDevice returned %08x\n", Status);
            ExReleaseResourceLite(&volume_list_lock);
            goto end;
        }
        
        volname.Length = volname.MaximumLength = (wcslen(BTRFS_VOLUME_PREFIX) + 36 + 1) * sizeof(WCHAR);
        volname.Buffer = ExAllocatePoolWithTag(PagedPool, volname.MaximumLength, ALLOC_TAG); // FIXME - when do we free this?
        
        if (!volname.Buffer) {
            ERR("out of memory\n");
            ExReleaseResourceLite(&volume_list_lock);
            goto end;
        }
        
        RtlCopyMemory(volname.Buffer, BTRFS_VOLUME_PREFIX, wcslen(BTRFS_VOLUME_PREFIX) * sizeof(WCHAR));
        
        j = wcslen(BTRFS_VOLUME_PREFIX);
        for (i = 0; i < 16; i++) {
            volname.Buffer[j] = hex_digit(sb->uuid.uuid[i] >> 4); j++;
            volname.Buffer[j] = hex_digit(sb->uuid.uuid[i] & 0xf); j++;
            
            if (i == 3 || i == 5 || i == 7 || i == 9) {
                volname.Buffer[j] = '-';
                j++;
            }
        }
        
        volname.Buffer[j] = '}';
        
        Status = IoCreateDevice(drvobj, sizeof(volume_device_extension), &volname, FILE_DEVICE_DISK, FILE_DEVICE_SECURE_OPEN, FALSE, &voldev);
        if (!NT_SUCCESS(Status)) {
            ERR("IoCreateDevice returned %08x\n", Status);
            ExReleaseResourceLite(&volume_list_lock);
            goto end;
        }
        
        voldev->StackSize = 2;
        voldev->SectorSize = sb->sector_size;
        
        vde = voldev->DeviceExtension;
        vde->type = VCB_TYPE_VOLUME;
        vde->uuid = sb->uuid;
        vde->name = volname;
        vde->device = voldev;
        vde->mounted_device = NULL;
        vde->pdo = pdo;
        
        ExInitializeResourceLite(&vde->child_lock);
        InitializeListHead(&vde->children);
        vde->num_children = sb->num_devices;
        vde->children_loaded = 0;
        
        new_vde = TRUE;
        
        Status = IoRegisterDeviceInterface(pdo, &GUID_DEVINTERFACE_VOLUME, NULL, &vde->bus_name);
        if (!NT_SUCCESS(Status))
            WARN("IoRegisterDeviceInterface returned %08x\n", Status);
        else
            registered = TRUE;
        
        vde->attached_device = IoAttachDeviceToDeviceStack(voldev, pdo);
        
        pdo->Flags &= ~DO_DEVICE_INITIALIZING;
        
        if (registered) {
            Status = IoSetDeviceInterfaceState(&vde->bus_name, TRUE);
            if (!NT_SUCCESS(Status))
                WARN("IoSetDeviceInterfaceState returned %08x\n", Status);
        }
    } else {
        ExAcquireResourceExclusiveLite(&vde->child_lock, TRUE);
        ExConvertExclusiveToSharedLite(&volume_list_lock);
        
        le = vde->children.Flink;
        while (le != &vde->children) {
            volume_child* vc2 = CONTAINING_RECORD(le, volume_child, list_entry);
            
            if (RtlCompareMemory(&vc2->uuid, &sb->dev_item.device_uuid, sizeof(BTRFS_UUID)) == sizeof(BTRFS_UUID)) {
                // duplicate, ignore
                ExReleaseResourceLite(&vde->child_lock);
                ExReleaseResourceLite(&volume_list_lock);
                goto end;
            }
            
            le = le->Flink;
        }
    }
    
    vc = ExAllocatePoolWithTag(PagedPool, sizeof(volume_child), ALLOC_TAG);
    if (!vc)
        ERR("out of memory\n");
    else {
        vc->uuid = sb->dev_item.device_uuid;
        vc->devid = sb->dev_item.dev_id;
        vc->generation = sb->generation;
        
        ObReferenceObject(DeviceObject);
        vc->devobj = DeviceObject;
        
        vc->pnp_name.Length = vc->pnp_name.MaximumLength = devpath->Length;
        vc->pnp_name.Buffer = ExAllocatePoolWithTag(PagedPool, devpath->Length, ALLOC_TAG);
        
        if (vc->pnp_name.Buffer)
            RtlCopyMemory(vc->pnp_name.Buffer, devpath->Buffer, devpath->Length);
        else {
            ERR("out of memory\n");
            vc->pnp_name.Length = vc->pnp_name.MaximumLength = 0;
        }
        
        vc->offset = offset;
        vc->size = length;
        vc->seeding = sb->flags & BTRFS_SUPERBLOCK_FLAGS_SEEDING ? TRUE : FALSE;
        vc->disk_num = disk_num;
        vc->part_num = part_num;
        
        InsertTailList(&vde->children, &vc->list_entry); // FIXME - these should be in order
        
        vde->children_loaded++;
    }
    
    if (!new_vde) {
        ExReleaseResourceLite(&vde->child_lock);
        ExReleaseResourceLite(&volume_list_lock);
    } else {
        InsertTailList(&volume_list, &vde->list_entry);
        
        voldev->Flags &= ~DO_DEVICE_INITIALIZING;
        
        ExReleaseResourceLite(&volume_list_lock);
    }
    
    remove_drive_letter(mountmgr, partname);
    
end:
    ObDereferenceObject(FileObject);
}