import { useState, useEffect } from 'react';
import './App.css';

function App() {
  const [lightState, setLightState] = useState<boolean | null>(null);
  const [password, setPassword] = useState('');
  const [error, setError] = useState('');
  const [darkMode, setDarkMode] = useState(true);
  const [isLoading, setIsLoading] = useState(true);

  useEffect(() => {
    fetchLightState();
  }, []);

  const fetchLightState = async () => {
    try {
      const response = await fetch('/lightSwitchState');
      const data = await response.json();
      setLightState(data);
    } catch (err) {
      setError('Failed to fetch light state');
    } finally {
      setIsLoading(false);
    }
  };

  const toggleLight = async () => {
    if (!password) {
      setError('Password is required');
      return;
    }

    setIsLoading(true);
    try {
      const response = await fetch('/lightSwitchState/toggle', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ password })
      });

      if (!response.ok) {
        throw new Error('Invalid password or server error');
      }

      const data = await response.json();
      setLightState(data);
      setError('');
    } catch (err) {
      setError((err as any).message);
    } finally {
      setIsLoading(false);
    }
  };

  const toggleDarkMode = () => {
    setDarkMode(!darkMode);
    if (darkMode) {
      alert('NERD!');
    }
  };

  return (
    <div
      className={`fixed inset-0 ${
        darkMode
          ? 'bg-gradient-to-br from-gray-900 via-purple-900 to-blue-900'
          : 'bg-gradient-to-br from-blue-100 via-purple-100 to-gray-100'
      }`}
    >
      <div className="absolute inset-0 flex items-center justify-center p-4">
        <div className="w-full max-w-md bg-white/10 backdrop-blur-lg rounded-xl shadow-2xl p-8">
          <h1
            className={`text-4xl font-bold mb-8 bg-gradient-to-r ${
              darkMode ? 'from-blue-400 to-purple-500' : 'from-blue-600 to-purple-700'
            } bg-clip-text text-transparent`}
          >
            Remote Light Switch
          </h1>

          <div className="space-y-6">
            {isLoading ? (
              <div className="flex items-center justify-center py-4">
                <div className="w-8 h-8 border-4 border-purple-500 border-t-transparent rounded-full animate-spin"></div>
              </div>
            ) : (
              <div className={`text-xl font-medium ${darkMode ? 'text-white' : 'text-gray-800'}`}>
                The light is currently{' '}
                <span className={`font-bold ${lightState ? 'text-yellow-400' : 'text-gray-400'}`}>
                  {lightState ? 'ON' : 'OFF'}
                </span>
              </div>
            )}

            <div className="space-y-4">
              <input
                type="password"
                placeholder="Enter password"
                value={password}
                onChange={e => setPassword(e.target.value)}
                className="w-full px-4 py-2 rounded-lg bg-white/20 border border-purple-500/30 backdrop-blur-sm text-white placeholder-purple-200 focus:outline-none focus:ring-2 focus:ring-purple-500"
              />

              <button
                onClick={toggleLight}
                disabled={isLoading}
                className="w-full py-3 px-6 bg-gradient-to-r from-blue-500 to-purple-600 hover:from-blue-600 hover:to-purple-700 text-white font-semibold rounded-lg shadow-lg transform transition-all hover:scale-[1.02] disabled:opacity-50 disabled:cursor-not-allowed"
              >
                Toggle Light
              </button>
            </div>

            <button
              onClick={toggleDarkMode}
              className="w-full py-2 px-4 bg-gradient-to-r from-gray-700 to-gray-900 hover:from-gray-800 hover:to-gray-950 text-white font-medium rounded-lg shadow-md transform transition-all hover:scale-[1.02]"
            >
              {darkMode ? '🌞 Light Mode (for nerds)' : '🌙 Dark Mode'}
            </button>

            {error && (
              <div className="text-red-400 text-sm bg-red-900/20 border border-red-500/30 rounded-lg p-3">{error}</div>
            )}
          </div>
        </div>
      </div>
    </div>
  );
}

export default App;
