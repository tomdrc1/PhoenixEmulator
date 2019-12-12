from PIL import Image

def main():
    with open("PhoenixEmulator\colors.txt", "r") as f:
        data = f.read().split("\n")
    
    img = Image.new("RGB", (8,0x400*8))
    print(len(data))
    for c in range(0x400):
        for i in range(8):
            for j in range(8):
                index = i * 8 + ((c * 8) + j)
                d = data[index][2::][::-1]
                img.putpixel((i, (c * 8) + j), int(d, 16))

    img.save("color7.png")


if __name__ == "__main__":
    main()