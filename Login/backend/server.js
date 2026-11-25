const express = require('express');
const cors = require('cors');
const bcrypt = require('bcryptjs');

const app = express();
const PORT = 5000;

app.use(cors());
app.use(express.json());

const users = new Map();

app.post('/api/register', (req, res) => {
  const { name, email } = req.body;
  if (!name || !email) {
    return res.status(400).json({ message: 'Name and email are required.' });
  }
  const normalizedEmail = email.trim().toLowerCase();
  if (users.has(normalizedEmail)) {
    return res.status(409).json({ message: 'Account already exists. Please login.' });
  }
  users.set(normalizedEmail, { name: name.trim(), email: normalizedEmail, passwordHash: null });
  return res.status(201).json({ message: 'Account created. Please set your password.' });
});

app.post('/api/set-password', async (req, res) => {
  const { email, password } = req.body;
  if (!email || !password || password.length < 6) {
    return res.status(400).json({ message: 'Password must be at least 6 characters.' });
  }
  const normalizedEmail = email.trim().toLowerCase();
  const user = users.get(normalizedEmail);
  if (!user) {
    return res.status(404).json({ message: 'Account not found. Please register first.' });
  }
  user.passwordHash = await bcrypt.hash(password, 10);
  return res.json({ message: 'Password saved. You can now login.' });
});

app.post('/api/login', async (req, res) => {
  const { email, password } = req.body;
  if (!email || !password) {
    return res.status(400).json({ message: 'Email and password are required.' });
  }
  const normalizedEmail = email.trim().toLowerCase();
  const user = users.get(normalizedEmail);
  if (!user) {
    return res.status(401).json({ message: 'Invalid email or password.' });
  }
  if (!user.passwordHash) {
    return res.status(400).json({ message: 'Please set your password first.', needsPasswordSetup: true });
  }
  const isMatch = await bcrypt.compare(password, user.passwordHash);
  if (!isMatch) {
    return res.status(401).json({ message: 'Invalid email or password.' });
  }
  return res.json({
    message: 'Login successful.',
    user: { name: user.name, email: user.email },
  });
});

app.listen(PORT, () => {
  console.log(`Backend running on http://localhost:${PORT}`);
});
