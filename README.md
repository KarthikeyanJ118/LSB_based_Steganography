# 🔐 LSB Steganography using C (BMP Image)

## 📌 Project Overview

This project implements **LSB (Least Significant Bit) Steganography** in C to securely hide and extract secret data inside BMP images.

The application supports both:

* **Encoding**: Hiding a secret file inside an image
* **Decoding**: Extracting the hidden data from the image

A **magic string authentication** mechanism is used to ensure secure access during decoding.

---

## ⚙️ Features

* Encode secret data into `.bmp` images
* Decode hidden data from stego images
* Supports any file type (via extension encoding)
* Magic string protection for secure decoding
* Efficient bit-level manipulation using LSB technique
* Command-line based execution

---

## 🛠️ Technologies Used

* C Programming
* File Handling (fopen, fread, fwrite)
* Bitwise Operations
* BMP Image Processing

---

## 🧠 How It Works

### 🔸 Encoding Process

1. Reads the source BMP image
2. Accepts a secret file and magic string
3. Checks if the image has enough capacity
4. Encodes:

   * Magic string size and data
   * File extension
   * Secret file size
   * Secret file data
5. Generates a **stego image** with hidden data

---

### 🔸 Decoding Process

1. Opens the stego image
2. Skips BMP header
3. Extracts magic string size and data
4. Verifies user-entered magic string
5. Extracts:

   * File extension
   * Secret data size
   * Secret data
6. Reconstructs the original secret file

---

## 📂 Project Structure

```
main.c
encode.c
encode.h
decode.c
decode.h
types.h
common.h
```

---

## ▶️ How to Compile

```
gcc *.c -o stego
```

---

## 🚀 How to Run

### 🔹 Encoding

```
./stego -e <input.bmp> <secret_file> [output.bmp]
```

Example:

```
./stego -e input.bmp secret.txt output.bmp
```

---

### 🔹 Decoding

```
./stego -d <stego.bmp> [output_file]
```

Example:

```
./stego -d output.bmp decoded.txt
```

---

## 🔐 Security Feature

* Uses a **Magic String** as a password
* Decoding proceeds only if the entered string matches the encoded one

---

## 📌 Example

Secret file:

```
My password is SECRET ;)
```

After encoding → hidden inside image
After decoding → original file is reconstructed

---

## 👨‍💻 Author

**Karthikeyan J**


