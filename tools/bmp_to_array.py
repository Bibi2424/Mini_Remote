import argparse
from PIL import Image

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--image-file', '-f', required=True, help='path to source img')
    parser.add_argument('--output-file', '-o', default=None, help='path to output file')
    parser.add_argument('--image-name', '-n', default='image', help='name to be used for array and define name, and file name if not specified')
    args = parser.parse_args()

    im = Image.open(args.image_file).convert('RGB')

    buffer = []
    for y in range(im.size[1]):
        for x in range(im.size[0]):
            r, g, b = im.getpixel((x,y))

            buffer.append( ((r >> 6) << 4) | ((g >> 6) << 2) | (b >> 6) )
            # print(buffer[-1])

    print(len(buffer))

    if not args.output_file:
        args.output_file = '{}.h'.format(args.image_name)
    with open(args.output_file, 'w') as f:
        f.write('#ifndef {}_H\n'.format(args.image_name.upper()))
        f.write('#define {}_H\n\n'.format(args.image_name.upper()))
        f.write("#include <avr/pgmspace.h>\n\n")
        f.write('#define {}_WIDTH   {}\n'.format(args.image_name.upper(), im.size[0]))
        f.write('#define {}_HEIGHT  {}\n\n'.format(args.image_name.upper(), im.size[1]))
        f.write("const uint8_t {}_array[80*72] PROGMEM = ".format(args.image_name.lower()) + '{\n')
        while buffer:
            size = min(len(buffer), 16)
            line = ', '.join(['0x{:02X}'.format(i) for i in buffer[0:size]]) + ','
            buffer = buffer[size:]
            f.write("\t{}\n".format(line))
        f.write('};\n\n')
        f.write('#endif\n')


if __name__ == "__main__":
    main()
