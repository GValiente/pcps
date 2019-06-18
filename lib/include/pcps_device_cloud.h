/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_DEVICE_CLOUD_H
#define PCPS_DEVICE_CLOUD_H

namespace pcps
{

class Cloud;
class Context;

/**
 * @brief Maps a Cloud object to a compute device.
 */
class DeviceCloud
{

public:
    /**
     * @brief Class contructor.
     * @param hostCloud Read-only host Cloud object.
     * @param context Compute context.
     */
    DeviceCloud(const Cloud& hostCloud, Context& context);

    /**
     * @brief Class contructor.
     * @param hostCloud Writeable host Cloud object.
     * @param context Compute context.
     */
    DeviceCloud(Cloud& hostCloud, Context& context);

    /**
     * @brief Copy construction is not allowed.
     */
    DeviceCloud(const DeviceCloud& other) = delete;

    /**
     * @brief Copy assignment is not allowed.
     */
    DeviceCloud& operator=(const DeviceCloud& other) = delete;

    /**
     * @brief Move constructor.
     * @param other Source object.
     */
    DeviceCloud(DeviceCloud&& other) noexcept;

    /**
     * @brief Move assignment operator.
     * @param other Source object.
     */
    DeviceCloud& operator=(DeviceCloud&& other) noexcept;

    /**
     * @brief Class destructor.
     */
    ~DeviceCloud() noexcept;

    /**
     * @brief Retrieves a read-only reference to the source host cloud.
     */
    const Cloud& getHostCloud() const noexcept
    {
        return *_hostCloud;
    }

    /**
     * @brief Updates the host cloud with the content of the device cloud.
     *
     * This operation only works if the source host cloud is writeable.
     *
     * @param context Compute context.
     * @return true if the operation was completed successfully; false otherwise.
     */
    bool updateHostCloud(Context& context);

    /**
     * @brief Retrieves a read-only reference to the points stored in the compute device.
     *
     * It can never be null.
     */
    const void* getDeviceData() const noexcept
    {
        return _deviceData;
    }

    /**
     * @brief Retrieves a writeable reference to the points stored in the compute device.
     *
     * It can be null if the source host cloud is read-only.
     */
    void* getDeviceData() noexcept;

private:
    ///@cond INTERNAL

    Cloud* _hostCloud;
    void* _deviceData;
    bool _constHostCloud;

    ///@endcond
};

}

#endif
