import React, { useState } from 'react';
import { useAuth } from '../../context/AuthContext';
import authService from '../../services/auth.service';
import { useNavigate } from 'react-router-dom';
import { toast } from 'react-hot-toast';

interface LoginFormProps {
  onSwitch: () => void;
}

const LoginForm: React.FC<LoginFormProps> = ({ onSwitch }) => {
  const [email, setEmail] = useState('');
  const [password, setPassword] = useState('');
  const [isLoading, setIsLoading] = useState(false);
  const { login } = useAuth();
  const navigate = useNavigate();

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setIsLoading(true);
    try {
      console.log("Attempting login with email:", email);
      const response = await authService.login({ email, password });
      console.log("Login response:", response);
      
      if (response.success) {
        // Map backend response to User type
        const userData = {
          id: response.data.userId || response.data.id,
          userId: response.data.userId,
          email: response.data.email,
          fullName: response.data.fullName,
          role: response.data.role?.toUpperCase(),
          isActive: true,
          createdAt: new Date().toISOString(),
          updatedAt: new Date().toISOString()
        };
        
        login(response.data.token, userData as any);
        toast.success("Welcome back!");
        navigate('/dashboard');
      } else {
        const errorMsg = typeof response.error === 'object' 
          ? response.error.message || "Login failed" 
          : String(response.error || "Login failed");
        toast.error(errorMsg);
      }
    } catch (error: any) {
      console.error("Login error:", error);
      const errorMsg = error?.message || (typeof error === 'string' ? error : "An unexpected error occurred");
      toast.error(String(errorMsg));
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <div className="p-8">
      <div className="text-center mb-8">
        <h2 className="text-3xl font-bold bg-gradient-to-r from-blue-400 to-purple-400 bg-clip-text text-transparent">
          Welcome Back
        </h2>
        <p className="text-slate-400 mt-2">Sign in to your account</p>
      </div>

      <form onSubmit={handleSubmit} className="space-y-6">
        <div>
          <label className="block text-sm font-semibold text-slate-700 mb-2">Email address</label>
          <input
            type="email"
            value={email}
            onChange={(e) => setEmail(e.target.value)}
            className="input-field"
            placeholder="example@gmail.com"
            required
          />
        </div>
        <div>
          <label className="block text-sm font-semibold text-slate-700 mb-2">Password</label>
          <input
            type="password"
            value={password}
            onChange={(e) => setPassword(e.target.value)}
            className="input-field"
            placeholder="········"
            required
          />
        </div>
        <div className="flex items-center justify-between">
          <label className="flex items-center gap-2 text-sm text-slate-600 cursor-pointer">
            <input type="checkbox" className="w-4 h-4 rounded border-slate-300 text-blue-600 focus:ring-blue-500" /> Remember Me
          </label>
          <a href="#" className="text-sm text-blue-600 hover:text-blue-700 font-semibold">Forgot Password?</a>
        </div>
        <button
          type="submit"
          disabled={isLoading}
          className="btn-primary w-full py-4 text-lg"
        >
          {isLoading ? 'Signing in...' : 'Sign In'}
        </button>
      </form>

      <div className="mt-8 text-center">
        <p className="text-slate-500">
          New to the platform?{' '}
          <button
            type="button"
            onClick={onSwitch}
            className="text-blue-600 hover:underline font-bold"
          >
            Create Account
          </button>
        </p>
        <p className="text-slate-400 text-[10px] mt-6 opacity-60 tracking-wider uppercase">
          🔐 SECURE ENCRYPTED CONNECTION
        </p>
      </div>
    </div>
  );
};

export default LoginForm;
