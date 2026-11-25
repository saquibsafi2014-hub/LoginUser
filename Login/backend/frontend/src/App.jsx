import { useState } from 'react';
import './App.css';
import ControlDashboard from './ControlDashboard';

const API_URL = 'http://localhost:5000/api';

export default function App() {
  // Steps: 'register' | 'set-password' | 'login' | 'dashboard'
  const [step, setStep] = useState('register');
  const [name, setName] = useState('');
  const [email, setEmail] = useState('');
  const [password, setPassword] = useState('');
  const [confirmPassword, setConfirmPassword] = useState('');
  const [status, setStatus] = useState({ type: '', text: '' });
  const [loggedInUser, setLoggedInUser] = useState(null);

  const handleRegister = async (e) => {
    e.preventDefault();
    setStatus({ type: '', text: '' });

    try {
      const res = await fetch(`${API_URL}/register`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ name, email }),
      });
      const data = await res.json();

      if (!res.ok) {
        return setStatus({ type: 'error', text: data.message });
      }
      setStatus({ type: 'success', text: data.message });
      setStep('set-password');
    } catch {
      setStatus({ type: 'error', text: 'Cannot reach backend on http://localhost:5000.' });
    }
  };

  const handleSetPassword = async (e) => {
    e.preventDefault();
    setStatus({ type: '', text: '' });

    if (password !== confirmPassword) {
      return setStatus({ type: 'error', text: 'Passwords do not match.' });
    }

    try {
      const res = await fetch(`${API_URL}/set-password`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ email, password }),
      });
      const data = await res.json();

      if (!res.ok) {
        return setStatus({ type: 'error', text: data.message });
      }
      setPassword('');
      setConfirmPassword('');
      setStatus({ type: 'success', text: data.message });
      setStep('login');
    } catch {
      setStatus({ type: 'error', text: 'Cannot reach backend on http://localhost:5000.' });
    }
  };

  const handleLogin = async (e) => {
    e.preventDefault();
    setStatus({ type: '', text: '' });

    try {
      const res = await fetch(`${API_URL}/login`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ email, password }),
      });
      const data = await res.json();

      if (!res.ok) {
        if (data.needsPasswordSetup) setStep('set-password');
        return setStatus({ type: 'error', text: data.message });
      }
      setLoggedInUser(data.user);
      setStatus({ type: '', text: '' });
      setStep('dashboard');
    } catch {
      setStatus({ type: 'error', text: 'Cannot reach backend on http://localhost:5000.' });
    }
  };

  const handleLogout = () => {
    setLoggedInUser(null);
    setPassword('');
    setStatus({ type: '', text: '' });
    setStep('login');
  };

  if (step === 'dashboard' && loggedInUser) {
    return <ControlDashboard user={loggedInUser} onLogout={handleLogout} />;
  }

  return (
    <div className="auth-wrapper">
      <div className="auth-card">
        <div className="stepper">
          <span className={step === 'register' ? 'active' : 'done'}>1. Create Account</span>
          <span className={step === 'set-password' ? 'active' : step === 'login' ? 'done' : ''}>
            2. Set Password
          </span>
          <span className={step === 'login' ? 'active' : ''}>3. Login</span>
        </div>

        {status.text && <div className={`alert ${status.type}`}>{status.text}</div>}

        {step === 'register' && (
          <form onSubmit={handleRegister}>
            <h2>Create Account</h2>
            <input
              type="text"
              placeholder="Full Name"
              value={name}
              onChange={(e) => setName(e.target.value)}
              required
            />
            <input
              type="email"
              placeholder="Email Address"
              value={email}
              onChange={(e) => setEmail(e.target.value)}
              required
            />
            <button type="submit">Continue to Set Password</button>
            <p className="switch-link">
              Already have an account?{' '}
              <span onClick={() => { setStatus({}); setStep('login'); }}>Login</span>
            </p>
          </form>
        )}

        {step === 'set-password' && (
          <form onSubmit={handleSetPassword}>
            <h2>Set Password</h2>
            <p className="subtext">Setting password for <strong>{email}</strong></p>
            <input
              type="password"
              placeholder="New Password (min 6 chars)"
              value={password}
              onChange={(e) => setPassword(e.target.value)}
              required
            />
            <input
              type="password"
              placeholder="Confirm Password"
              value={confirmPassword}
              onChange={(e) => setConfirmPassword(e.target.value)}
              required
            />
            <button type="submit">Save Password</button>
          </form>
        )}

        {step === 'login' && (
          <form onSubmit={handleLogin}>
            <h2>Login</h2>
            <input
              type="email"
              placeholder="Email Address"
              value={email}
              onChange={(e) => setEmail(e.target.value)}
              required
            />
            <input
              type="password"
              placeholder="Password"
              value={password}
              onChange={(e) => setPassword(e.target.value)}
              required
            />
            <button type="submit">Sign In</button>
            <p className="switch-link">
              New user?{' '}
              <span onClick={() => { setStatus({}); setStep('register'); }}>Create Account</span>
            </p>
          </form>
        )}
      </div>
    </div>
  );
}