import Dither from 'canvas-dither'
import './index.css'

const button = document.getElementById('go-button')
const canvas = document.getElementById('canvas')
const form = document.getElementById('form')
const input = document.getElementById('input')
const { height, width } = canvas

const context = canvas.getContext('2d')
context.font = '30px Arial'
context.fillStyle = 'black'
context.fillRect(0, 0, width, height)
context.fillStyle = 'white'
context.fillText('Hello World', 10, 50)

const drawImage = image => {
  const scale = Math.max(width / image.width, height / image.height)
  const x = width / 2 - (image.width / 2) * scale
  const y = height / 2 - (image.height / 2) * scale

  context.drawImage(image, x, y, image.width * scale, image.height * scale)

  let data = context.getImageData(0, 0, width, height)
  data = Dither.atkinson(data)
  context.putImageData(data, 0, 0)
}

input.onchange = event => {
  const [file] = event.target.files
  const image = new Image()
  image.onload = () => {
    drawImage(image)
  }
  image.onerror = () => {
    console.error('Could not load image')
  }
  image.src = URL.createObjectURL(file)
}

button.onclick = function onClick() {
  const imageData = context.getImageData(0, 0, width, height)

  const dithered = Dither.atkinson(imageData)

  const data = []
  for (let i = 0; i < dithered.data.length; i += 4) {
    const pixel = dithered.data[i] + dithered.data[i + 1] + dithered.data[i + 2]
    const colour = pixel > 128 ? 0 : 1
    data.push(colour)
  }

  const formData = new FormData(form)
  formData.append('data', data.join(''))
  formData.append('x', 0)
  formData.append('y', 0)
  formData.append('width', width)
  formData.append('height', height)

  fetch('/pixels', {
    method: 'POST',
    body: formData,
  })
}
