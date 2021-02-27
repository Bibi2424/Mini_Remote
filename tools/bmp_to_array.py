import argparse
from PIL import Image

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--image-file', '-f', required=True, help='path to source img')
    parser.add_argument('--output-file', '-o', default='custom_image.h', help='path to output file')
    args = parser.parse_args()

    im = Image.open(args.image_file).convert('RGB')

    buffer = []
    for y in range(im.size[1]):
        for x in range(im.size[0]):
            r, g, b = im.getpixel((x,y))

            buffer.append( ((r >> 6) << 4) | ((g >> 6) << 2) | (b >> 6) )
            # print(buffer[-1])

    print(len(buffer))

    with open(args.output_file, 'w') as f:
        f.write('#ifndef BUFFER_H\n')
        f.write('#define BUFFER_H\n\n')
        f.write("#include <avr/pgmspace.h>\n\n")
        f.write('#define IMAGE_WIDTH   {}\n'.format(im.size[0]))
        f.write('#define IMAGE_HEIGHT  {}\n\n'.format(im.size[1]))
        f.write("const uint8_t image_array[80*72] PROGMEM = {\n")
        while buffer:
            size = min(len(buffer), 16)
            line = ', '.join(['0x{:02X}'.format(i) for i in buffer[0:size]]) + ','
            buffer = buffer[size:]
            f.write("\t{}\n".format(line))
        f.write('};\n\n')
        f.write('#endif\n')


if __name__ == "__main__":
    main()
