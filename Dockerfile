# Use the official Ubuntu 20.04 as a base image
FROM ubuntu:22.04

# Set the working directory
WORKDIR /vectordb

ENV TZ=Asia/Jakarta
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Update package lists and install necessary dependencies
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    cmake \
    libboost-all-dev \
    wget

# Copy the source files to the working directory
COPY . .

# Build the application
RUN mkdir build && \
    cd build && \
    cmake .. && \
    make

# Expose the port the application will run on
EXPOSE 3737

# Run the application
CMD ["/vectordb/build/vectordb"]