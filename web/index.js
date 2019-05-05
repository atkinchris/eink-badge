const button = document.getElementById('go-button')
const canvas = document.getElementById('canvas')
const context = canvas.getContext('2d')
const form = document.getElementById('form')
const { height, width } = canvas

context.font = '30px Arial'
context.fillStyle = 'black'
context.fillRect(0, 0, width, height)
context.fillStyle = 'white'
context.fillText('Hello World', 10, 50)

button.onclick = function onClick() {
  const pixels = context.getImageData(0, 0, width, height)

  const data = []
  for (let i = 0; i < pixels.data.length; i += 4) {
    const pixel = pixels.data[i] + pixels.data[i + 1] + pixels.data[i + 2]
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
