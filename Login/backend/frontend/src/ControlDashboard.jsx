import { useState } from 'react';
import './ControlDashboard.css';

const NAV_ITEMS = [
  'Control',
  'Switch Matrix',
  'Power & Temperature',
  'Command Logs',
  'Settings',
  'About',
];

const INITIAL_SWITCH_HISTORY = [
  { id: 1, timestamp: '21-Oct-2026 14:28:15', category: 'Switch Control', command: 'Connect', route: 'SRC 1 -> DEST 4', status: 'Success', responseMs: 25, message: 'Command executed successfully' },
  { id: 2, timestamp: '21-Oct-2026 14:27:50', category: 'Switch Control', command: 'Disconnect', route: 'SRC 2 -> DEST 3', status: 'Success', responseMs: 22, message: 'Command executed successfully' },
  { id: 3, timestamp: '21-Oct-2026 14:26:40', category: 'Switch Control', command: 'Connect', route: 'SRC 3 -> DEST 1', status: 'Success', responseMs: 24, message: 'Command executed successfully' },
  { id: 4, timestamp: '21-Oct-2026 14:25:10', category: 'Switch Control', command: 'Disconnect', route: 'SRC 1 -> DEST 2', status: 'Success', responseMs: 20, message: 'Command executed successfully' },
  { id: 5, timestamp: '21-Oct-2026 14:24:35', category: 'Switch Control', command: 'Connect', route: 'SRC 4 -> DEST 2', status: 'Success', responseMs: 23, message: 'Command executed successfully' },
];

const INITIAL_POWER_HISTORY = [
  { id: 1, timestamp: '21-Oct-2026 14:32:20', command: 'Read Temperature', parameter: 'Temperature', value: '46.2 °C', status: 'Success', responseMs: 19 },
  { id: 2, timestamp: '21-Oct-2026 14:32:14', command: 'Read 5V Status', parameter: '5V Supply', value: '5.02 V', status: 'Success', responseMs: 21 },
  { id: 3, timestamp: '21-Oct-2026 14:32:10', command: 'Read 28V Status', parameter: '28V Supply', value: '28.1 V', status: 'Success', responseMs: 23 },
  { id: 4, timestamp: '21-Oct-2026 14:30:05', command: 'Read Temperature', parameter: 'Temperature', value: '45.8 °C', status: 'Success', responseMs: 20 },
  { id: 5, timestamp: '21-Oct-2026 14:29:50', command: 'Read 5V Status', parameter: '5V Supply', value: '5.01 V', status: 'Success', responseMs: 19 },
  { id: 6, timestamp: '21-Oct-2026 14:29:45', command: 'Read 28V Status', parameter: '28V Supply', value: '28.0 V', status: 'Success', responseMs: 21 },
];

function formatNow() {
  const d = new Date();
  const months = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'];
  const day = String(d.getDate()).padStart(2, '0');
  const mon = months[d.getMonth()];
  const yr = d.getFullYear();
  const hh = String(d.getHours()).padStart(2, '0');
  const mm = String(d.getMinutes()).padStart(2, '0');
  const ss = String(d.getSeconds()).padStart(2, '0');
  return `${day}-${mon}-${yr} ${hh}:${mm}:${ss}`;
}

export default function ControlDashboard({ user, onLogout }) {
  const [activeNav, setActiveNav] = useState('Control');
  const [category, setCategory] = useState('Switch Control');
  const [commandName, setCommandName] = useState('Connect');
  const [route, setRoute] = useState('SRC 1 -> DEST 4');

  const [currentCmd, setCurrentCmd] = useState({
    category: 'Switch Control',
    command: 'Connect',
    route: 'SRC 1 -> DEST 4',
    status: 'Success',
    responseMs: 25,
    timestamp: '21-Oct-2026 14:28:15',
  });

  const [previousCmd, setPreviousCmd] = useState({
    category: 'Switch Control',
    command: 'Disconnect',
    route: 'SRC 2 -> DEST 3',
    status: 'Success',
    responseMs: 25,
    timestamp: '21-Oct-2026 14:27:50',
  });

  const [latestResponse, setLatestResponse] = useState({
    category: 'Switch Control',
    command: 'Connect',
    route: 'SRC 1 -> DEST 4',
    status: 'Success',
    responseMs: 25,
    timestamp: '21-Oct-2026 14:28:15',
    message: 'Command executed successfully',
  });

  const [v28, setV28] = useState({ status: 'ON', voltage: '28.1 V', ms: 23, ts: '21-Oct-2026 14:32:10' });
  const [v5, setV5] = useState({ status: 'ON', voltage: '5.02 V', ms: 21, ts: '21-Oct-2026 14:32:14' });
  const [temp, setTemp] = useState({ value: '46.2 °C', ms: 19, ts: '21-Oct-2026 14:32:20' });

  const [switchHistory, setSwitchHistory] = useState(INITIAL_SWITCH_HISTORY);
  const [powerHistory, setPowerHistory] = useState(INITIAL_POWER_HISTORY);

  const handleSendCommand = (e) => {
    e.preventDefault();
    const ts = formatNow();
    const ms = Math.floor(Math.random() * 8) + 19;

    setPreviousCmd({ ...currentCmd });
    const nextCmd = { category, command: commandName, route, status: 'Success', responseMs: ms, timestamp: ts };
    setCurrentCmd(nextCmd);
    setLatestResponse({ ...nextCmd, message: 'Command executed successfully' });

    setSwitchHistory((prev) => [
      { id: prev.length + 1, timestamp: ts, category, command: commandName, route, status: 'Success', responseMs: ms, message: 'Command executed successfully' },
      ...prev,
    ]);
  };

  const handleClear = () => {
    setCategory('Switch Control');
    setCommandName('Connect');
    setRoute('SRC 1 -> DEST 4');
  };

  const handleRequest28V = () => {
    const ts = formatNow();
    const ms = Math.floor(Math.random() * 6) + 20;
    const voltage = `${(27.9 + Math.random() * 0.4).toFixed(1)} V`;
    setV28({ status: 'ON', voltage, ms, ts });
    setPowerHistory((prev) => [
      { id: prev.length + 1, timestamp: ts, command: 'Read 28V Status', parameter: '28V Supply', value: voltage, status: 'Success', responseMs: ms },
      ...prev,
    ]);
  };

  const handleRequest5V = () => {
    const ts = formatNow();
    const ms = Math.floor(Math.random() * 6) + 19;
    const voltage = `${(4.99 + Math.random() * 0.05).toFixed(2)} V`;
    setV5({ status: 'ON', voltage, ms, ts });
    setPowerHistory((prev) => [
      { id: prev.length + 1, timestamp: ts, command: 'Read 5V Status', parameter: '5V Supply', value: voltage, status: 'Success', responseMs: ms },
      ...prev,
    ]);
  };

  const handleRequestTemp = () => {
    const ts = formatNow();
    const ms = Math.floor(Math.random() * 6) + 17;
    const value = `${(45.5 + Math.random() * 1.5).toFixed(1)} °C`;
    setTemp({ value, ms, ts });
    setPowerHistory((prev) => [
      { id: prev.length + 1, timestamp: ts, command: 'Read Temperature', parameter: 'Temperature', value, status: 'Success', responseMs: ms },
      ...prev,
    ]);
  };

  return (
    <div className="console-shell">
      <aside className="console-sidebar">
        <div className="sidebar-brand">FPGA Console</div>
        <nav className="sidebar-nav">
          {NAV_ITEMS.map((item) => (
            <button
              key={item}
              type="button"
              className={`nav-btn ${activeNav === item ? 'active' : ''}`}
              onClick={() => setActiveNav(item)}
            >
              {item}
            </button>
          ))}
        </nav>

        <div className="sidebar-footer">
          <div className="operator-info">
            <span className="operator-name">{user?.name || 'Operator'}</span>
            <span className="operator-email">{user?.email || ''}</span>
          </div>
          <button type="button" className="logout-btn" onClick={onLogout}>
            Logout
          </button>
        </div>
      </aside>

      <div className="console-main">
        <header className="telemetry-bar">
          <div className="telemetry-item">
            <span className="label">UDP Connection</span>
            <span className="value status-green">Connected</span>
          </div>
          <div className="telemetry-item">
            <span className="label">FPGA IP</span>
            <span className="value">192.168.1.100</span>
          </div>
          <div className="telemetry-item">
            <span className="label">UDP Port</span>
            <span className="value">5000</span>
          </div>
          <div className="telemetry-item">
            <span className="label">Local IP</span>
            <span className="value">192.168.1.10</span>
          </div>
          <div className="telemetry-item">
            <span className="label">Local Port</span>
            <span className="value">5000</span>
          </div>
          <div className="telemetry-item">
            <span className="label">System Status</span>
            <span className="value status-green">Normal</span>
          </div>
        </header>

        <div className="top-panels">
          <section className="panel cmd-panel">
            <div className="panel-header">Switch Matrix Command</div>
            <form className="cmd-form" onSubmit={handleSendCommand}>
              <label className="form-row">
                <span>Command Category</span>
                <select value={category} onChange={(e) => setCategory(e.target.value)}>
                  <option value="Switch Control">Switch Control</option>
                  <option value="Matrix Reset">Matrix Reset</option>
                </select>
              </label>

              <label className="form-row">
                <span>Command Name</span>
                <select value={commandName} onChange={(e) => setCommandName(e.target.value)}>
                  <option value="Connect">Connect</option>
                  <option value="Disconnect">Disconnect</option>
                </select>
              </label>

              <label className="form-row">
                <span>Source -&gt; Destination</span>
                <select value={route} onChange={(e) => setRoute(e.target.value)}>
                  <option value="SRC 1 -> DEST 4">SRC 1 -&gt; DEST 4</option>
                  <option value="SRC 2 -> DEST 3">SRC 2 -&gt; DEST 3</option>
                  <option value="SRC 3 -> DEST 1">SRC 3 -&gt; DEST 1</option>
                  <option value="SRC 4 -> DEST 2">SRC 4 -&gt; DEST 2</option>
                </select>
              </label>

              <div className="cmd-actions">
                <button type="submit" className="btn-primary">Send Command</button>
                <button type="button" className="btn-secondary" onClick={handleClear}>Clear</button>
              </div>
            </form>
          </section>

          <section className="panel monitor-panel">
            <div className="panel-header">Independent Monitoring Commands</div>
            <div className="monitor-grid">
              <div className="monitor-card">
                <div className="monitor-title">28V Power Supply Status</div>
                <button type="button" className="btn-action" onClick={handleRequest28V}>
                  Request 28V Status
                </button>
                <div className="kv-row"><span>Status</span><strong className="status-green">{v28.status}</strong></div>
                <div className="kv-row"><span>Voltage</span><strong>{v28.voltage}</strong></div>
                <div className="kv-row"><span>Response Time</span><strong>{v28.ms} ms</strong></div>
                <div className="kv-row"><span>Timestamp</span><strong>{v28.ts}</strong></div>
              </div>

              <div className="monitor-card">
                <div className="monitor-title">5V Power Supply Status</div>
                <button type="button" className="btn-action" onClick={handleRequest5V}>
                  Request 5V Status
                </button>
                <div className="kv-row"><span>Status</span><strong className="status-green">{v5.status}</strong></div>
                <div className="kv-row"><span>Voltage</span><strong>{v5.voltage}</strong></div>
                <div className="kv-row"><span>Response Time</span><strong>{v5.ms} ms</strong></div>
                <div className="kv-row"><span>Timestamp</span><strong>{v5.ts}</strong></div>
              </div>

              <div className="monitor-card">
                <div className="monitor-title">FPGA Temperature</div>
                <button type="button" className="btn-action" onClick={handleRequestTemp}>
                  Request Temperature
                </button>
                <div className="kv-row"><span>Temperature</span><strong>{temp.value}</strong></div>
                <div className="kv-row"><span>Response Time</span><strong>{temp.ms} ms</strong></div>
                <div className="kv-row"><span>Timestamp</span><strong>{temp.ts}</strong></div>
              </div>
            </div>
          </section>
        </div>

        <div className="mid-panels">
          <section className="panel">
            <div className="panel-header">Current Switch Matrix Command</div>
            <div className="summary-list">
              <div className="kv-row"><span>Category</span><strong>{currentCmd.category}</strong></div>
              <div className="kv-row"><span>Command</span><strong>{currentCmd.command}</strong></div>
              <div className="kv-row"><span>Source -&gt; Destination</span><strong>{currentCmd.route}</strong></div>
              <div className="kv-row"><span>Status</span><strong className="status-green">{currentCmd.status}</strong></div>
              <div className="kv-row"><span>Response Time</span><strong>{currentCmd.responseMs} ms</strong></div>
              <div className="kv-row"><span>Timestamp</span><strong>{currentCmd.timestamp}</strong></div>
            </div>
          </section>

          <section className="panel">
            <div className="panel-header">Previous Switch Matrix Command</div>
            <div className="summary-list">
              <div className="kv-row"><span>Category</span><strong>{previousCmd.category}</strong></div>
              <div className="kv-row"><span>Command</span><strong>{previousCmd.command}</strong></div>
              <div className="kv-row"><span>Source -&gt; Destination</span><strong>{previousCmd.route}</strong></div>
              <div className="kv-row"><span>Status</span><strong className="status-green">{previousCmd.status}</strong></div>
              <div className="kv-row"><span>Response Time</span><strong>{previousCmd.responseMs} ms</strong></div>
              <div className="kv-row"><span>Timestamp</span><strong>{previousCmd.timestamp}</strong></div>
            </div>
          </section>

          <section className="panel">
            <div className="panel-header">Latest Response</div>
            <div className="summary-list">
              <div className="kv-row"><span>Category</span><strong>{latestResponse.category}</strong></div>
              <div className="kv-row"><span>Command</span><strong>{latestResponse.command}</strong></div>
              <div className="kv-row"><span>Source -&gt; Destination</span><strong>{latestResponse.route}</strong></div>
              <div className="kv-row"><span>Status</span><strong className="status-green">{latestResponse.status}</strong></div>
              <div className="kv-row"><span>Response Time</span><strong>{latestResponse.responseMs} ms</strong></div>
              <div className="kv-row"><span>Timestamp</span><strong>{latestResponse.timestamp}</strong></div>
              <div className="kv-row"><span>Message</span><strong>{latestResponse.message}</strong></div>
            </div>
          </section>
        </div>

        <div className="bottom-panels">
          <section className="panel">
            <div className="panel-header">Switch Matrix Command History</div>
            <div className="table-wrap">
              <table>
                <thead>
                  <tr>
                    <th>#</th>
                    <th>Timestamp</th>
                    <th>Category</th>
                    <th>Command Name</th>
                    <th>Source -&gt; Destination</th>
                    <th>Status</th>
                    <th>Response Time (ms)</th>
                    <th>Message</th>
                  </tr>
                </thead>
                <tbody>
                  {switchHistory.map((row, idx) => (
                    <tr key={row.id}>
                      <td>{idx + 1}</td>
                      <td>{row.timestamp}</td>
                      <td>{row.category}</td>
                      <td>{row.command}</td>
                      <td>{row.route}</td>
                      <td className="status-green">{row.status}</td>
                      <td>{row.responseMs}</td>
                      <td>{row.message}</td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          </section>

          <section className="panel">
            <div className="panel-header">Power &amp; Temperature Command History</div>
            <div className="table-wrap">
              <table>
                <thead>
                  <tr>
                    <th>#</th>
                    <th>Timestamp</th>
                    <th>Command</th>
                    <th>Parameter</th>
                    <th>Value</th>
                    <th>Status</th>
                    <th>Response Time (ms)</th>
                  </tr>
                </thead>
                <tbody>
                  {powerHistory.map((row, idx) => (
                    <tr key={row.id}>
                      <td>{idx + 1}</td>
                      <td>{row.timestamp}</td>
                      <td>{row.command}</td>
                      <td>{row.parameter}</td>
                      <td>{row.value}</td>
                      <td className="status-green">{row.status}</td>
                      <td>{row.responseMs}</td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          </section>
        </div>
      </div>
    </div>
  );
}