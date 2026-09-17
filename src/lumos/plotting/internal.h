#ifndef DUOPLOT_INTERNAL_H_
#define DUOPLOT_INTERNAL_H_

#include "socket_compat.h"

#include <chrono>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

#include "lumos/plotting/communication_header.h"
#include "lumos/plotting/constants.h"
#include "lumos/plotting/fillable_uint8_array.h"
#include "lumos/plotting/property_set.h"
#include "lumos/plotting/timing.h"
#include "lumos/plotting/utils.h"
#include "lumos/math.h"

namespace lumos
{
    namespace internal
    {

        class UdpClient
        {
        private:
            int sock_file_descr_;
            struct sockaddr_in tx_addr_;
            struct sockaddr *tx_addr_ptr_;

        public:
            UdpClient() : sock_file_descr_(-1), tx_addr_ptr_(nullptr) {}

            UdpClient(const uint64_t port_num)
            {
                if ((sock_file_descr_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
                {
                    throw std::runtime_error("socket failed!");
                }

                std::memset(&tx_addr_, 0, sizeof(tx_addr_));
                tx_addr_.sin_family = AF_INET;
                tx_addr_.sin_port = htons(port_num);
                tx_addr_.sin_addr.s_addr = htonl(INADDR_ANY);
                tx_addr_ptr_ = (struct sockaddr *)&tx_addr_;
            }

            ~UdpClient()
            {
                socket_close(sock_file_descr_);
            }

            template <int N>
            int receiveData(char data[N]) const
            {
                socklen_t client_len = sizeof(tx_addr_);
                const int num_received_bytes = recvfrom(sock_file_descr_, data, N, 0, (struct sockaddr *)&tx_addr_, &client_len);
                return num_received_bytes;
            }

            void sendData(const uint8_t *const data, const uint64_t num_bytes) const
            {
                if (sock_file_descr_ == -1)
                {
                    throw std::runtime_error("Invalid socket!");
                }
                else if (sendto(sock_file_descr_, (const char*)data, num_bytes, 0, tx_addr_ptr_, sizeof(tx_addr_)) < 0)
                {
                    throw std::runtime_error("sendto failed!");
                }
            }
        };

        using SendFunctionType = std::function<void(const UInt8ArrayView &input_array, const std::uint64_t port_num)>;

        inline int &getSocketFileDescriptor()
        {
            static int sock_file_descr = -1;
            return sock_file_descr;
        }

        inline bool &getIsInitialized()
        {
            static bool is_initialized = false;
            return is_initialized;
        }

        inline void initializeTcpSocket(const uint64_t port_num)
        {
            int &tcp_sockfd = getSocketFileDescriptor();
            struct sockaddr_in tcp_servaddr;

            tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0);

            bzero(&tcp_servaddr, sizeof(tcp_servaddr));

            tcp_servaddr.sin_family = AF_INET;
            tcp_servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            tcp_servaddr.sin_port = htons(port_num);

            if (connect(tcp_sockfd, (struct sockaddr *)&tcp_servaddr, sizeof(tcp_servaddr)) == (-1))
            {
                LOG_WARNING() << "Failed to connect! Is receiving application running?";
                return;
            }
        }

        inline void sendThroughTcpInterfaceClient(const UInt8ArrayView &input_array, const uint64_t port_num)
        {
            if (!getIsInitialized())
            {
                initializeTcpSocket(port_num);
                getIsInitialized() = true;
            }

            int &tcp_sockfd = getSocketFileDescriptor();

            const uint64_t num_bytes_to_send = input_array.size();

            socket_send(tcp_sockfd, &num_bytes_to_send, sizeof(uint64_t));
            socket_send(tcp_sockfd, input_array.data(), input_array.size());
        }

        inline void sendThroughTcpInterface(const UInt8ArrayView &input_array, const uint64_t port_num)
        {
            int tcp_sockfd;
            struct sockaddr_in tcp_servaddr;

            tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0);

            bzero(&tcp_servaddr, sizeof(tcp_servaddr));

            tcp_servaddr.sin_family = AF_INET;
            tcp_servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            tcp_servaddr.sin_port = htons(port_num);

            if (connect(tcp_sockfd, (struct sockaddr *)&tcp_servaddr, sizeof(tcp_servaddr)) == (-1))
            {
                LOG_WARNING() << "Failed to connect! Is receiving application running?";
                return;
            }

            const uint64_t num_bytes_to_send = input_array.size();

            socket_send(tcp_sockfd, &num_bytes_to_send, sizeof(uint64_t));
            socket_send(tcp_sockfd, input_array.data(), input_array.size());

            socket_close(tcp_sockfd);
        }

        inline void sendThroughQueryUdpInterface(const UInt8ArrayView &input_array)
        {
            const uint8_t *const data_to_send = input_array.data();
            const uint64_t total_num_bytes_to_send = input_array.size();

            constexpr int kNumReceiveBytes = 10;
            const UdpClient udp_client(kUdpQueryPortNum);
            char received_data[kNumReceiveBytes];

            if (total_num_bytes_to_send >= kMaxNumBytesForOneTransmission)
            {
                throw std::runtime_error("Tried to send too much data through query interface!");
            }

            udp_client.sendData(data_to_send, total_num_bytes_to_send);

            const int num_received_bytes = udp_client.receiveData<kNumReceiveBytes>(received_data);

            if (!ackValid(received_data))
            {
                throw std::runtime_error("No valid ack received!");
            }
            else if (num_received_bytes != 5)
            {
                throw std::runtime_error("Ack received but number of bytes was " + std::to_string(num_received_bytes));
            }
        }

        inline size_t receiveFromQueryUdpInterface()
        {
            constexpr int kNumReceiveBytes = 100;
            const UdpClient udp_client(kUdpQueryPortNum);
            char received_data[kNumReceiveBytes];
            std::cout << "Before" << std::endl;
            udp_client.receiveData<kNumReceiveBytes>(received_data);
            std::cout << "After" << std::endl;

            size_t received_size;

            std::memcpy(&received_size, received_data, sizeof(size_t));

            return received_size;
        }

        inline SendFunctionType getSendFunction()
        {
            return sendThroughTcpInterfaceClient;
        }

        template <typename U>
        void countNumBytes(uint64_t &num_bytes, const U &data_to_be_sent)
        {
            num_bytes += data_to_be_sent.numBytes();
        }

        template <typename U, typename... Us>
        void countNumBytes(uint64_t &num_bytes, const U &data_to_be_sent, const Us &...datas)
        {
            num_bytes += data_to_be_sent.numBytes();
            countNumBytes(num_bytes, datas...);
        }

        template <typename... Us>
        uint64_t countNumBytes(const Us &...datas)
        {
            uint64_t num_bytes = 0;
            countNumBytes(num_bytes, datas...);
            return num_bytes;
        }

        template <typename U>
        void fillBuffer(FillableUInt8Array &fillable_array, const U &data_to_be_sent)
        {
            fillable_array.fillWithDataFromPointer(data_to_be_sent.data(), data_to_be_sent.numElements());
        }

        template <typename U, typename... Us>
        void fillBuffer(FillableUInt8Array &fillable_array, const U &data_to_be_sent, const Us &...other_elements)
        {
            fillable_array.fillWithDataFromPointer(data_to_be_sent.data(), data_to_be_sent.numElements());
            fillBuffer(fillable_array, other_elements...);
        }

        template <typename U>
        void sendHeaderAndData(const SendFunctionType &send_function, const CommunicationHeader &hdr, const U &first_element)
        {
            const uint64_t num_bytes_hdr = hdr.numBytes();
            const uint64_t num_bytes_first = first_element.numBytes();

            const uint64_t num_bytes_from_object = num_bytes_hdr + num_bytes_first;

            // + 1 bytes for endianness byte
            // + 2 * sizeof(uint64_t) for magic number and number of bytes in transfer
            const uint64_t num_bytes = num_bytes_from_object + 1 + 2 * sizeof(uint64_t);

            FillableUInt8Array fillable_array{num_bytes};

            fillable_array.fillWithStaticType(isBigEndian());

            fillable_array.fillWithStaticType(kMagicNumber);

            fillable_array.fillWithStaticType(num_bytes);

            hdr.fillBufferWithData(fillable_array);

            fillable_array.fillWithDataFromPointer(first_element.data(), first_element.numElements());

            send_function(fillable_array.view(), kTcpPortNum);
        }

        template <typename U, typename... Us>
        void sendHeaderAndData(const SendFunctionType &send_function,
                               const CommunicationHeader &hdr,
                               const U &first_element,
                               const Us &...other_elements)
        {
            const uint64_t num_bytes_hdr = hdr.numBytes();
            const uint64_t num_bytes_first = first_element.numBytes();

            const uint64_t num_bytes_from_object = countNumBytes(other_elements...) + num_bytes_hdr + num_bytes_first;

            // + 1 bytes for endianness byte
            // + 2 * sizeof(uint64_t) for magic number and number of bytes in transfer
            const uint64_t num_bytes = num_bytes_from_object + 1 + 2 * sizeof(uint64_t);

            FillableUInt8Array fillable_array{num_bytes};

            fillable_array.fillWithStaticType(isBigEndian());

            fillable_array.fillWithStaticType(kMagicNumber);

            fillable_array.fillWithStaticType(num_bytes);

            hdr.fillBufferWithData(fillable_array);

            fillable_array.fillWithDataFromPointer(first_element.data(), first_element.numElements());

            fillBuffer(fillable_array, other_elements...);

            send_function(fillable_array.view(), kTcpPortNum);
        }

        template <typename U>
        void fillBufferWithCollection(FillableUInt8Array &fillable_array, const U &data_to_be_sent)
        {
            for (size_t k = 0; k < data_to_be_sent.size(); k++)
            {
                fillable_array.fillWithDataFromPointer(data_to_be_sent[k].data(), data_to_be_sent[k].numElements());
            }
        }

        template <typename U, typename... Us>
        void fillBufferWithCollection(FillableUInt8Array &fillable_array, const U &data_to_be_sent, const Us &...other_elements)
        {
            for (size_t k = 0; k < data_to_be_sent.size(); k++)
            {
                fillable_array.fillWithDataFromPointer(data_to_be_sent[k].data(), data_to_be_sent[k].numElements());
            }

            fillBufferWithCollection(fillable_array, other_elements...);
        }

        template <typename U, typename... Us>
        void sendHeaderAndVectorCollection(const SendFunctionType &send_function,
                                           const CommunicationHeader &hdr,
                                           const Vector<uint16_t> &vector_lengths,
                                           const size_t num_bytes_to_send,
                                           const U &first_element,
                                           const Us &...other_elements)
        {
            const uint64_t num_bytes_hdr = hdr.numBytes();
            const uint64_t num_bytes_from_object = num_bytes_to_send + num_bytes_hdr;

            // + 1 bytes for endianness byte
            // + 2 * sizeof(uint64_t) for magic number and number of bytes in transfer
            const uint64_t num_bytes = num_bytes_from_object + 1 + 2 * sizeof(uint64_t) + vector_lengths.numBytes();

            FillableUInt8Array fillable_array{num_bytes};

            fillable_array.fillWithStaticType(isBigEndian());

            fillable_array.fillWithStaticType(kMagicNumber);

            fillable_array.fillWithStaticType(num_bytes);

            hdr.fillBufferWithData(fillable_array);

            fillable_array.fillWithDataFromPointer(vector_lengths.data(), vector_lengths.numElements());

            for (size_t k = 0; k < first_element.size(); k++)
            {
                fillable_array.fillWithDataFromPointer(first_element[k].data(), first_element[k].numElements());
            }

            fillBufferWithCollection(fillable_array, other_elements...);

            send_function(fillable_array.view(), kTcpPortNum);
        }

        inline void sendHeaderOnly(const SendFunctionType &send_function, const CommunicationHeader &hdr)
        {
            const uint64_t num_bytes_hdr = hdr.numBytes();

            // + 1 bytes for endianness byte
            // + 2 * sizeof(uint64_t) for magic number and number of bytes in transfer
            const uint64_t num_bytes = num_bytes_hdr + 1 + 2 * sizeof(uint64_t);

            FillableUInt8Array fillable_array{num_bytes};

            fillable_array.fillWithStaticType(isBigEndian());

            fillable_array.fillWithStaticType(kMagicNumber);

            fillable_array.fillWithStaticType(num_bytes);

            hdr.fillBufferWithData(fillable_array);

            send_function(fillable_array.view(), kTcpPortNum);
        }

        inline bool isDuoplotRunning()
        {
#ifdef _WIN32
            return false;
#else
            char path[1035];

            FILE *const fp = popen("ps -ef | grep duoplot", "r");
            if (fp == NULL)
            {
                LOG_ERROR() << "Failed to run command";
                return false;
            }

            std::vector<std::string> lines;
            while (fgets(path, sizeof(path), fp) != NULL)
            {
                lines.push_back(path);
            }

            bool duoplot_running = false;

            const auto ends_with = [](const std::string &full_string, const std::string &ending)
            {
                if (full_string.length() >= ending.length())
                {
                    return (0 == full_string.compare(full_string.length() - ending.length(), ending.length(), ending));
                }
                else
                {
                    return false;
                }
            };

            for (const std::string &line : lines)
            {
                if ((line.length() > 0U) && (line.find("duoplot") != std::string::npos) &&
                    (line.find("grep") == std::string::npos) && (ends_with(line, "duoplot\n") || ends_with(line, "duoplot &\n")))
                {
                    duoplot_running = true;
                }
            }

            pclose(fp);

            return duoplot_running;
#endif  /* _WIN32 */
        }

    } // namespace internal
} // namespace lumos

#endif // DUOPLOT_INTERNAL_H_
